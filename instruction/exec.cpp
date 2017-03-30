#include <assert.h>
#include "instruction/instruction.hpp"

bool ExecInstr::exec(void){
        if(!instrfuncs.count(OPCODE)){
                ERROR("not implemented OPCODE 0x%02x", OPCODE);
		return false;
	}

        (this->*instrfuncs[OPCODE])();
	return true;
}

void ExecInstr::set_rm32(uint32_t value){
	if(MOD == 3)
		SET_GPREG(static_cast<reg32_t>(RM), value);
	else 
		WRITE_MEM32(calc_modrm(), value);
}

uint32_t ExecInstr::get_rm32(void){
	if(MOD == 3)
		return GET_GPREG(static_cast<reg32_t>(RM));
	else
		return READ_MEM32(calc_modrm());
}

void ExecInstr::set_r32(uint32_t value){
	SET_GPREG(static_cast<reg32_t>(REG), value);
}

uint32_t ExecInstr::get_r32(void){
	return GET_GPREG(static_cast<reg32_t>(REG));
}

void ExecInstr::set_rm16(uint16_t value){
	if(MOD == 3)
		SET_GPREG(static_cast<reg16_t>(RM), value);
	else
		WRITE_MEM16(calc_modrm(), value);
}

uint16_t ExecInstr::get_rm16(void){
	if(MOD == 3)
		return GET_GPREG(static_cast<reg16_t>(RM));
	else
		return READ_MEM16(calc_modrm());
}

void ExecInstr::set_r16(uint16_t value){
	SET_GPREG(static_cast<reg16_t>(REG), value);
}

uint16_t ExecInstr::get_r16(void){
	return GET_GPREG(static_cast<reg16_t>(REG));
}

void ExecInstr::set_rm8(uint8_t value){
	if(MOD == 3)
		SET_GPREG(static_cast<reg8_t>(RM), value);
	else 
		WRITE_MEM8(calc_modrm(), value);
}

uint8_t ExecInstr::get_rm8(void){
	if(MOD == 3)
		return GET_GPREG(static_cast<reg8_t>(RM));
	else
		return READ_MEM8(calc_modrm());
}

void ExecInstr::set_r8(uint8_t value){
	SET_GPREG(static_cast<reg8_t>(REG), value);
}

uint8_t ExecInstr::get_r8(void){
	return GET_GPREG(static_cast<reg8_t>(REG));
}

uint32_t ExecInstr::get_m(void){
	return calc_modrm();
}

void ExecInstr::set_sreg(uint16_t value){
	EMU->set_sgreg(static_cast<sgreg_t>(REG), value);
}

uint16_t ExecInstr::get_sreg(void){
	return EMU->get_sgreg(static_cast<sgreg_t>(REG));
}

void ExecInstr::set_crn(uint32_t value){
	EMU->set_crn(REG, value);
}

uint32_t ExecInstr::get_crn(void){
	return EMU->get_crn(REG);
}

uint32_t ExecInstr::calc_modrm(void){
	assert(MOD != 3);

	if(is_protected() ^ chsz_ad)
		return calc_modrm32();
	else
		return calc_modrm16();
}

uint32_t ExecInstr::calc_modrm16(void){
	uint32_t addr = 0;

	switch(MOD){
		case 1:
			addr += DISP8;
			break;
		case 2:
			addr += DISP16;
			break;
	}

	switch(RM){
		case 0:
		case 1:
		case 7:
			addr += GET_GPREG(BX);
			break;
		case 2:
		case 3:
		case 6:
			if(MOD == 0 && RM == 6)
				addr += DISP16;
			else
				addr += GET_GPREG(BP);
			break;
	}

	if(RM < 6){
		if(RM % 2)
			addr += GET_GPREG(DI);
		else
			addr += GET_GPREG(SI);
	}

	return addr;
}

uint32_t ExecInstr::calc_modrm32(void){
	uint32_t addr = 0;

	switch(MOD){
		case 1:
			addr += DISP8;
			break;
		case 2:
			addr += DISP32;
			break;
	}

	switch(RM){
		case 4:
			addr += calc_sib();
			break;
		case 5:
			if(MOD == 0){
				addr += DISP32;
				break;
			}
		default:
			addr += GET_GPREG(static_cast<reg32_t>(RM));
	}

	return addr;
}

uint32_t ExecInstr::calc_sib(void){
	uint32_t base = 0;

	if(BASE == 5 && MOD == 0)
		base += DISP32;
	else if(BASE != 4)
		base += GET_GPREG(static_cast<reg32_t>(BASE));
	else if(INDEX != 4 || SCALE != 0)	// BASE == 4, INDEX ==4, SCALE == 0 : [esp]
		ERROR("not implemented SIB (base = %d, index = %d, scale = %d)\n", BASE, INDEX, SCALE);

	return base + GET_GPREG(static_cast<reg32_t>(INDEX)) * (1<<SCALE);
/*
	if(BASE!=5 && INDEX!=4)
		return GET_GPREG(static_cast<reg32_t>(BASE)) + GET_GPREG(static_cast<reg32_t>(INDEX)) * (1<<SCALE);
	else if(BASE==4 && INDEX==4 && SCALE==0)
		return GET_GPREG(static_cast<reg32_t>(ESP));
//	else if(BASE==5 && MOD==0)
//		return DISP32 + GET_GPREG(static_cast<reg32_t>(INDEX)) * (1<<SCALE);
*/
}

