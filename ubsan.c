#include "memory.h"

#include "ubsan.h"

static void printLocation(SourceLocation location)
{
	memory.debugMsg("       in %s, at line %u, column %u\n", location.fileName, location.line, location.column);
}

void __ubsan_handle_load_invalid_value(InvalidValueData *data, void *value)
{
	memory.debugMsg("UBSAN: invalid value of type %s\n", data->type->name);
	printLocation(data->location);
}

void __ubsan_handle_nonnull_arg(NonnullArgData *data)
{
	memory.debugMsg("UBSAN: null pointer passed as argument %u, which is declared to never be null\n", data->argidx);
	printLocation(data->location);
}

void __ubsan_handle_nullability_arg(NonnullArgData *data)
{
	memory.debugMsg("UBSAN: null pointer passed as argument %u, which is declared to never be null\n", data->argidx);
	printLocation(data->location);
}

void __ubsan_handle_nonnull_return_v1(NonnullReturnData *data, SourceLocation *location)
{
	memory.debugMsg("UBSAN: null pointer return from function declared to never return null\n");
	printLocation(*location);
}

void __ubsan_handle_nullability_return_v1(NonnullReturnData *data, SourceLocation *location)
{
	memory.debugMsg("UBSAN: null pointer return from function declared to never return null\n");
	printLocation(*location);
}

void __ubsan_handle_vla_bound_not_positive(VLABoundData *data, void *value)
{
	memory.debugMsg("UBSAN: VLA bound not positive of type %s\n", data->type->name);
	printLocation(data->location);
}

void __ubsan_handle_add_overflow(OverflowData *data, void *leftValue, void *rightValue)
{
	memory.debugMsg("UBSAN: addition overflow of type %s\n", data->type->name);
	printLocation(data->location);
}

void __ubsan_handle_sub_overflow(OverflowData *data, void *leftValue, void *rightValue)
{
	memory.debugMsg("UBSAN: substraction overflow of type %s\n", data->type->name);
	printLocation(data->location);
}

void __ubsan_handle_negate_overflow(OverflowData *data, void *value)
{
	memory.debugMsg("UBSAN: negation overflow of type %s\n", data->type->name);
	printLocation(data->location);
}

void __ubsan_handle_mul_overflow(OverflowData *data, void *leftValue, void *rightValue)
{
	memory.debugMsg("UBSAN: multiplication overflow of type %s\n", data->type->name);
	printLocation(data->location);
}

void __ubsan_handle_shift_out_of_bounds(ShiftOutOfBoundsData *data, void *leftValue, void *rightValue)
{
	memory.debugMsg("UBSAN: shift out of bounds, %s shifted by %s\n", data->leftType->name, data->rightType->name);
	printLocation(data->location);
}

void __ubsan_handle_divrem_overflow(OverflowData *data, void *leftValue, void *rightValue)
{
	memory.debugMsg("UBSAN: divrem overflow of type %s\n", data->type->name);
	printLocation(data->location);
}

void __ubsan_handle_out_of_bounds(OutOfBoundsData *data, void *value)
{
	memory.debugMsg("UBSAN: out of bounds access into array of type %s\n", data->arrayType->name);
	printLocation(data->location);
}

void __ubsan_handle_type_mismatch_v1(TypeMismatchData *data, void *p)
{
	memory.debugMsg("UBSAN: type mismatch\n");
	printLocation(data->location);
}

void __ubsan_handle_alignment_assumption(AlignmentAssumptionData *data, void *p, void *alignment, void *offset)
{
	memory.debugMsg("UBSAN: alignment assumption\n");
	printLocation(data->location);
}

void __ubsan_handle_builtin_unreachable(UnreachableData *data)
{
	memory.debugMsg("UBSAN: execution reached an unreachable program point\n");
	printLocation(data->location);
}

void __ubsan_handle_missing_return(UnreachableData *data)
{
	memory.debugMsg("UBSAN: execution reached the end of a value-returning function without returning a value\n");
	printLocation(data->location);
}

void __ubsan_handle_implicit_conversion(ImplicitConversionData *data, void *leftValue, void *rightValue)
{
	memory.debugMsg("UBSAN: implicit conversion from type %s to type %s\n", data->fromType->name, data->toType->name);
	printLocation(data->location);
}

void __ubsan_handle_invalid_builtin(InvalidValueData data)
{
	memory.debugMsg("UBSAN: passing invalid argument\n");
	printLocation(data.location);
}

void __ubsan_handle_pointer_overflow(PointerOverflowData *data, void *base, void *result)
{
	memory.debugMsg("UBSAN: pointer overflow\n");
	printLocation(data->location);
}
