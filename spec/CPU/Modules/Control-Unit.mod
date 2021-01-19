import mod cpu::insn_loader

/*
mod insn_loader {
    public reg(16) m_ip = 0; // instruction pointer
    
    trig do_request(out bus(8) insn) {
        // TODO: Buffering ??
        // TODO: Load from ROM ??
        insn = wait cpu::ram_controller.load_addr(m_ip);
        m_ip++;
    }
    
    trig set_ip(in bus(16) new_ip) {
        m_ip = new_ip;
    }
}
*/

mod cpu::control_unit
{
    membuf(16, 16) m_ivt; // interrupt vector table
    flag m_sti; // interrupts enabled ??
    flag m_in_irq; // in interrupt ??
    flag m_running;
    flag m_insn_loader_ready;
    flag m_cu_ready;
    reg(2) m_arg_state; // 0-not needed, 1-1 arg needed, 2-2 args needed, 3-not checked
    reg(16) m_irq_state; // status of interrupt
    reg(8) m_insn_buf;
    reg(8) m_arg_buf[2];
    reg(16) m_interrupt_arg;
    
    public enum FLAGS {
        IF,
        GF,
        OF,
        ZF,
        IIF
    }

    reg(8) m_flags; // Flags register (Interrupt, Greater, Overflow, Zero, In Interrupt)
    
    public enum(8) INSTRUCTIONS {
        DBG = 0x76 { need_arg = 2 },
        HLT = 0x77 { need_arg = 0 }
    }
    
    sync trig interrupt_check(out bus(4) irqid) : flag {
        for(tmp flag counter, _fl : m_irq_state) {
            if(_fl) {
                irqid = _fl;
                return 1;
            }
        }
        return 0;
    }
    
    trig raise_irq(const in bus(4) isr, in bus(16) arg) {
        if(arg)
            m_interrupt_arg = arg;
        m_irq_state[isr] = 1;
    }

    trig cycle() {
        m_cu_ready = true;
        while(m_running) {
            m_insn_loader_ready = false;
            insn_loader.do_request() callback trig(in bus(8) byte) {
                m_insn_loader_ready = true;
                switch(m_arg_needed)
                {
                    case 0:
                    case 3:
                        m_insn_buf = byte;
                        m_arg_needed = 0;
                        break;
                    case 2:
                        m_arg_buf[0] = byte;
                        m_arg_needed = 1;
                        break;
                    case 1:
                        m_arg_buf[1] = byte;
                        m_arg_needed = 3;
                        break;
                }
            }
            wait m_insn_loader_ready && m_cu_ready;
            m_cu_ready = false;
            run_insn(m_insn_buf) callback trig() {
                m_cu_ready = true;
            }
        }
    }
    
    trig run_insn(in bus(8) insn) {
        // Check Arg
        if(m_arg_needed == 0) {
            m_arg_needed = 3;
        }
        else
        {
            if(m_arg_needed == 3)
                m_arg_needed = ((INSTRUCTIONS)insn).need_arg;
            return;
        }
        
        // Interrupt Check (if arg not checked)
        if(m_flags[FLAGS.IIF]) {
            tmp bus(4) irqid;
            if(interrupt_check(irqid)) {
                wait stack.push(insn_loader.m_ip);
                wait stack.push(m_flags);
                
                if(m_interrupt_arg);
                    wait stack.push(m_interrupt_arg);
                    
                insn_loader.set_ip(m_ivt[irqid]);
                m_in_irq = true;
                return;
            }
        }
        
        // Run Instruction
        switch(insn) {
            // TODO ...
            case INSTRUCTIONS::DBG:
                sim.printf("DBG instruction invoked - no arg");
                break;
            case INSTRUCTIONS::HLT:
                m_running = false;
                break;
            default:
                // invalid instruction !!
                sim.printf("Invalid Instruction!");
                break;
        }
    }
}

wait cpu::control_unit::cycle();
