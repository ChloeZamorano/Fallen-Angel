```cpp
struct BinaHeader
{
	char Signature[4] = "BINA";
	char Version[3] = "210"; // I'm fairly sure this can be 110
							 //but I haven't checked older games.
	char EndianFlag; // L for little or B for big. Can the games
					 // parse endianness opposite to the platform's?
					 // TODO: Test this later.

	u32 FileSize; // This doesn't need to be the size of the actual
				  // file, which means you can have a file bigger
				  // than this says and place data the game will
				  // never read at the end of the file.

	u16 NodeCount; // It seems like this is always either 1 or 0, this
				   // leads me to think it might actually be a boolean
				   // and be something like HasContents or something.

	u16 Unknown = 0; // Could be padding, nobody knows. I intent to
					 // look into it at some point.

	DataNode Node; // Invalid if NodeCount is 0.

	// If FileSize is smaller than the actual size of the file then
	// you can insert a "secret message" after the entirety of the
	// node's content. It can be anything really, the game will never
	// be aware of it and to my knowledge it will cause no issues.
	u8[OS_FileSize - FileSize] ExtraData;
};

struct Node
{
	char Signature[4] = "DATA";
	u32 Length;	// Apparently it's always FileSize - 16 but I wanna
				// double check it.
};

struct DataNode	// There doesn't seem to be any other type of node
				// so in my implementation I don't have a Node type,
				// I only have the DataNode directly.
{
	Node Head;
	u32 StringTableOffset;	// Relative to the Data array, it can be
							// practically thought of as the length of
							// the data buffer.
	u32 StringTableLength;
	u32 OffsetTableLength;
	u16 AdditionalDataLength = 24;	// This is always 24, but I don't
									// know if changing it will mess
									// anything up.
	u16 Padding = 0;

	u8[AdditionalDataLength] AdditionalData; // Purpose is unknown, but
											 // it always contains just
											 // nulls.

	u8[StringTableOffset] Data;	// This is a raw buffer to be parsed as
								// the file type this Bina container is
								// meant to represent.

	u8[StringTableLength] StringsTable;	// A collection of strings packed
										// to one buffer, separated by
										// null bytes. The buffer is
										// terminated by a null u32.

	// This is the most complicated part.
	// This is essentially an array of variable integers.
	// * The first two bits tell you the size or "Command":
	//  - 00: An instruction that the table ended.
	//  - 01: One byte offset.
	//  - 10: Two byte offset.
	//  - 11: Four byte offset.
	// * The rest of the bits are the actual value of the
	//   offset.
	// * The value of the offset is shifted to the right by
	//   two, which means all offsets are a multiple of 4,
	//   but the intended effect is that you can't have an
	//   offset lower than 4.
	// * The first offset is relative to where the additional
	//   data ends, which is 64 bytes after the file start.
	// * Every other offset is relative to where the last one
	//   pointed.
	u8[OffsetTableLength] OffsetsTable;
};
```