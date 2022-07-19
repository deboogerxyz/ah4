#include <stdint.h>

typedef struct {
	const char *fileName;
	uint32_t line;
	uint32_t column;
} SourceLocation;

typedef struct {
	uint16_t kind;
	uint16_t info;
	char name[1];
} TypeDescriptor;

typedef struct {
	SourceLocation location;
	TypeDescriptor *type;
} InvalidValueData;

typedef struct {
	SourceLocation location;
	SourceLocation attributeLocation;
	int argidx;
} NonnullArgData;

typedef struct {
	SourceLocation attributeLocation;
} NonnullReturnData;

typedef struct {
	SourceLocation location;
	TypeDescriptor *type;
} OverflowData;

typedef struct {
	SourceLocation location;
	TypeDescriptor *type;
} VLABoundData;

typedef struct {
	SourceLocation location;
	TypeDescriptor *leftType;
	TypeDescriptor *rightType;
} ShiftOutOfBoundsData;

typedef struct {
	SourceLocation location;
	TypeDescriptor *arrayType;
	TypeDescriptor *indexType;
} OutOfBoundsData;

typedef struct {
	SourceLocation location;
	TypeDescriptor *type;
	uint8_t logAlignment;
	uint8_t typeCheckKind;
} TypeMismatchData;

typedef struct {
	SourceLocation location;
	SourceLocation assumptionLocation;
	TypeDescriptor *type;
} AlignmentAssumptionData;

typedef struct {
	SourceLocation location;
} UnreachableData;

typedef struct {
	SourceLocation location;
	TypeDescriptor *fromType;
	TypeDescriptor *toType;
	unsigned char kind;
} ImplicitConversionData;

typedef struct {
	SourceLocation location;
	unsigned char kind;
} InvalidBuiltinData;

typedef struct {
	SourceLocation location;
} PointerOverflowData;

typedef struct {
	SourceLocation location;
	TypeDescriptor *fromType;
	TypeDescriptor *toType;
} FloatCastOverflowData;
