#!/usr/bin/env python

all_insn = [
    "BRK b",
    "ORA (d,X)",
    "COP b",
    "ORA d,S",
    "TSB d",
    "ORA d",
    "ASL d",
    "ORA [d]",
    "PHP",
    "ORA #",
    "ASL A",
    "PHD",
    "TSB a",
    "ORA a",
    "ASL a",
    "ORA al",
    "BPL r",
    "ORA (d),Y",
    "ORA (d)",
    "ORA (d,S),Y",
    "TRB d",
    "ORA d,X",
    "ASL d,X",
    "ORA [d],Y",
    "CLC",
    "ORA a,Y",
    "INC A",
    "TCS",
    "TRB a",
    "ORA a,X",
    "ASL a,X",
    "ORA al,X",
    "JSR a",
    "AND (d,X)",
    "JSL al",
    "AND d,S",
    "BIT d",
    "AND d",
    "ROL d",
    "AND [d]",
    "PLP",
    "AND #",
    "ROL A",
    "PLD",
    "BIT a",
    "AND a",
    "ROL a",
    "AND al",
    "BMI r",
    "AND (d),Y",
    "AND (d)",
    "AND (d,S),Y",
    "BIT d,X",
    "AND d,X",
    "ROL d,X",
    "AND [d],Y",
    "SEC",
    "AND a,Y",
    "DEC A",
    "TSC",
    "BIT a,X",
    "AND a,X",
    "ROL a,X",
    "AND al,X",
    "RTI",
    "EOR (d,X)",
    "WDM",
    "EOR d,S",
    "MVP s,d",
    "EOR d",
    "LSR d",
    "EOR [d]",
    "PHA",
    "EOR #",
    "LSR A",
    "PHK",
    "JMP a",
    "EOR a",
    "LSR a",
    "EOR al",
    "BVC r",
    "EOR (d),Y",
    "EOR (d)",
    "EOR (d,S),Y",
    "MVN s,d",
    "EOR d,X",
    "LSR d,X",
    "EOR [d],Y",
    "CLI",
    "EOR a,Y",
    "PHY",
    "TCD",
    "JMP al",
    "EOR a,X",
    "LSR a,X",
    "EOR al,X",
    "RTS",
    "ADC (d,X)",
    "PER rl",
    "ADC d,S",
    "STZ d",
    "ADC d",
    "ROR d",
    "ADC [d]",
    "PLA",
    "ADC #",
    "ROR A",
    "RTL",
    "JMP (a)",
    "ADC a",
    "ROR a",
    "ADC al",
    "BVS r",
    "ADC (d),Y",
    "ADC (d)",
    "ADC (d,S),Y",
    "STZ d,X",
    "ADC d,X",
    "ROR d,X",
    "ADC [d],Y",
    "SEI",
    "ADC a,Y",
    "PLY",
    "TDC",
    "JMP (a,X)",
    "ADC a,X",
    "ROR a,X",
    "ADC al,X",
    "BRA r",
    "STA (d,X)",
    "BRL rl",
    "STA d,S",
    "STY d",
    "STA d",
    "STX d",
    "STA [d]",
    "DEY",
    "BIT #",
    "TXA",
    "PHB",
    "STY a",
    "STA a",
    "STX a",
    "STA al",
    "BCC r",
    "STA (d),Y",
    "STA (d)",
    "STA (d,S),Y",
    "STY d,X",
    "STA d,X",
    "STX d,Y",
    "STA [d],Y",
    "TYA",
    "STA a,Y",
    "TXS",
    "TXY",
    "STZ a",
    "STA a,X",
    "STZ a,X",
    "STA al,X",
    "LDY #",
    "LDA (d,X)",
    "LDX #",
    "LDA d,S",
    "LDY d",
    "LDA d",
    "LDX d",
    "LDA [d]",
    "TAY",
    "LDA #",
    "TAX",
    "PLB",
    "LDY a",
    "LDA a",
    "LDX a",
    "LDA al",
    "BCS r",
    "LDA (d),Y",
    "LDA (d)",
    "LDA (d,S),Y",
    "LDY d,X",
    "LDA d,X",
    "LDX d,Y",
    "LDA [d],Y",
    "CLV",
    "LDA a,Y",
    "TSX",
    "TYX",
    "LDY a,X",
    "LDA a,X",
    "LDX a,Y",
    "LDA al,X",
    "CPY #",
    "CMP (d,X)",
    "REP #",
    "CMP d,S",
    "CPY d",
    "CMP d",
    "DEC d",
    "CMP [d]",
    "INY",
    "CMP #",
    "DEX",
    "WAI",
    "CPY a",
    "CMP a",
    "DEC a",
    "CMP al",
    "BNE r",
    "CMP (d),Y",
    "CMP (d)",
    "CMP (d,S),Y",
    "PEI d",
    "CMP d,X",
    "DEC d,X",
    "CMP [d],Y",
    "CLD",
    "CMP a,Y",
    "PHX",
    "STP",
    "JML (a)",
    "CMP a,X",
    "DEC a,X",
    "CMP al,X",
    "CPX #",
    "SBC (d,X)",
    "SEP #",
    "SBC d,S",
    "CPX d",
    "SBC d",
    "INC d",
    "SBC [d]",
    "INX",
    "SBC #",
    "NOP",
    "XBA",
    "CPX a",
    "SBC a",
    "INC a",
    "SBC al",
    "BEQ r",
    "SBC (d),Y",
    "SBC (d)",
    "SBC (d,S),Y",
    "PEA a",
    "SBC d,X",
    "INC d,X",
    "SBC [d],Y",
    "SED",
    "SBC a,Y",
    "PLX",
    "XCE",
    "JSR (a,X)",
    "SBC a,X",
    "INC a,X",
    "SBC al,X" ]

for i in xrange(0x100):
    cur_insn = all_insn[i]
    cur_mnem = cur_insn[:cur_insn.find(' ')].lower()
    cur_oprd = cur_insn[cur_insn.find(' ')+1:]

    cur_oper = ''

    print 'case 0x%02x: /* %s */' % (i, cur_insn) + '',

    if 0:
        pass

    elif cur_mnem == 'brk':
        cur_oper = 'Break'

    elif cur_mnem == 'inc':
        cur_oper = 'Add'
        cur_oprd += ',1'

    elif cur_mnem == 'dec':
        cur_oper = 'Sub'
        cur_oprd += ',1'

    elif cur_mnem == 'adc':
        pass
        #cur_oper = 'AddWithCarry'

    elif cur_mnem == 'sbc':
        pass
        #cur_oper = 'SubWithBorrow'

    elif cur_mnem == 'cmp':
        pass
        #cur_oper = 'Compare'

    elif cur_mnem == 'bit':
        pass
        #cur_oper = 'Bit'

    elif cur_mnem == 'rts':
        pass
        #cur_oper = 'ReturnFromSub'

    elif cur_mnem == 'rtl':
        pass
        #cur_oper = 'ReturnFromSubLong'

    elif cur_mnem == 'rti':
        pass
        #cur_oper = 'ReturnFromInterrupt'

    elif cur_mnem.startswith('ld'):
        cur_oper = 'Load'
        cur_oprd = cur_mnem[-1] + cur_oprd

    elif cur_mnem.startswith('st'):
        cur_oper = 'Store'
        cur_oprd = cur_mnem[-1] + cur_oprd

    elif cur_mnem.startswith('cp'):
        pass
        #cur_oper = 'Compare'
        #cur_oprd = cur_mnem[-1] + cur_oprd

    elif cur_mnem.startswith('or'):
        cur_oper = 'Or'
        cur_oprd = cur_mnem[-1] + cur_oprd

    elif cur_mnem.startswith('ph'):
        pass
        #cur_oper = 'Push'
        #cur_oprd = cur_mnem[-1] + cur_oprd

    elif cur_mnem.startswith('in'):
        cur_oper = 'Add'
        cur_oprd = cur_mnem[-1] + ',1'

    elif cur_mnem.startswith('de'):
        cur_oper = 'Sub'
        cur_oprd = cur_mnem[-1] + ',1'

    oprds = cur_oprd.split(',')
    print ' /* %s */' % oprds + '',
    named_oprds = []
    #get_reg = 'GetRegister(pCpuReadVal, pCpuCtxtObjVal, %s)'

    if len(oprds) == 2:
        for oprd in oprds:
            if oprd == 'a' or oprd == 'al':
                named_oprds.append('REG_A')
            elif oprd == 'b':
                named_oprds.append('REG_B')
            elif oprd == 'X':
                named_oprds.append('REG_X')
            elif oprd == 'Y':
                named_oprds.append('REG_Y')
            elif oprd == '1':
                named_oprds.append('1')
            else:
                cur_oper = ''
    else:
        cur_oper = ''

    if len(cur_oper):
        print '  %s(pCpuReadVal, pCpuWriteVal, pCpuCtxtObjVal, %s);' % (cur_oper, ', '.join(named_oprds)) + '',
    else:
        print '  /* unhandled operation %s */' % cur_insn + '',

    print('  break;')
