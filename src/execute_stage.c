#include <math.h>
#include <stdio.h>
#include "execute_stage.h"
#include "core.h"

/**
 * The function is needed for alu operations.
 * It treats the registers as signed integers and not as unsigned ones.
 */
int32_t uintToInt(uint32_t num)
{
    int32_t ret = 1;

    if (num >= (uint32_t)pow(2, 31))
    {
        int64_t tmp = 0;
        // sign2 complement:
        tmp = - (int64_t)pow(2,31) + (int64_t)(num - (uint32_t)pow(2,31));
        ret = (int32_t)tmp;
    }
    else
    {
        ret = (int32_t)num;
    }

    return ret;
}

/**
 * We wish to save the result as a uint.
 */
uint32_t intTouint(int32_t num)
{
    return (uint32_t)num;
}

/**
 * A right shift on a signed number is undefined by the c standard.
 * Therefore, an implementation such as (int)num >> shift
 * shall be incorrect. We need to implement it ourselves.
 */
uint32_t arithmeticShiftRight(uint32_t num, uint32_t shiftCount)
{
    if (num >= 0)
    {
        // An arithmetic shift is identical to logical shift in such case
        return num >> shiftCount;
    }
    if (shiftCount > 31)
        return -1;

    for (uint32_t i = 0; i < shiftCount; i++)
    {
        num /= 2;
        num += pow(2,31);
    }
    return num;
}

void performALUOperation(ExecuteStage* self)
{
    switch (self->state.inputState.instruction.opcode) {
        case Add:
            self->state.outputState.aluOperationOutput =
                intTouint(uintToInt(self->state.inputState.rsValue) +
                          uintToInt(self->state.inputState.rtValue));
            break;
        case Sub:
            self->state.outputState.aluOperationOutput =
                intTouint(uintToInt(self->state.inputState.rsValue) -
                          uintToInt(self->state.inputState.rtValue));
            break;
        case Mul:
            self->state.outputState.aluOperationOutput =
                intTouint(uintToInt(self->state.inputState.rsValue) *
                          uintToInt(self->state.inputState.rtValue));
            break;
        case And:
            self->state.outputState.aluOperationOutput =
                self->state.inputState.rsValue & self->state.inputState.rtValue;
            break;
        case Or:
            self->state.outputState.aluOperationOutput =
                self->state.inputState.rsValue | self->state.inputState.rtValue;
            break;
        case Xor:
            self->state.outputState.aluOperationOutput =
                self->state.inputState.rsValue ^ self->state.inputState.rtValue;
            break;
        case Sll:
            self->state.outputState.aluOperationOutput =
                self->state.inputState.rsValue << self->state.inputState.rtValue;
            break;
        case Sra:
            self->state.outputState.aluOperationOutput =
                arithmeticShiftRight(self->state.inputState.rsValue, self->state.inputState.rtValue);
            break;
        case Srl:
            self->state.outputState.aluOperationOutput =
                self->state.inputState.rsValue >> self->state.inputState.rtValue;
            break;
        case Lw:
        case Sw:
            self->state.outputState.aluOperationOutput =
                self->state.inputState.rsValue + self->state.inputState.rtValue;
            break;
        default:
            break;
    }
}

bool do_execute_operation(ExecuteStage* self)
{
    // First copy the output state from the input state.
    // Later update the output state with operation needed to be
    // done at this round
    self->state.outputState = self->state.inputState;

    performALUOperation(self);

    return false;       // Execute stage never stalls
}

void configure_execute_stage(ExecuteStage* stage, struct core* myCore)
{
    configure_stage_data(&(stage->state), myCore);
}