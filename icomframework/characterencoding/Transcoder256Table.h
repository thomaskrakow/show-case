
#if !defined(ICOM_FRAMEWORK_TRANSCODER256TABLE)
#define ICOM_FRAMEWORK_TRANSCODER256TABLE

#include "Transcoder.h"
#include "ICOMFrameworkTypes.h"


namespace ICOM{ 
 namespace Framework{

	class  Transcoder256Table : public Transcoder
	{
	public :	
	
		struct Transcoder256TableRecord
		{
			ICF_uint32       intCh;
			char     		 extCh;
		};
		
		virtual ~Transcoder256Table();
		
		virtual int		transcodeFrom(const char* inputBytes,int inputLengthInBytes,char* const outputUTF32,int availableOutputLengthInBytes);
		virtual int		transcodeTo(const char* const inputUTF32,int inputLengthInBytes,char* const outputBytes,int availableOutputLengthInBytes);
		
		virtual int	getMaxBytesPerCharacter();
		virtual int	getMinBytesPerCharacter();
		virtual int getLengthOfNullCharacter();

	protected:
	
		// -----------------------------------------------------------------------
		//  Hidden constructors
		// -----------------------------------------------------------------------
		Transcoder256Table
		(
			const ICF_uint32* const                        fromTable
			, const Transcoder256Table::Transcoder256TableRecord* const    toTable
			, const int                           toTableSize
		);

		
	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------
		Transcoder256Table(const Transcoder256Table&);
		Transcoder256Table& operator=(const Transcoder256Table&);
		
		char 		xlatOneTo(const ICF_uint32 toXlat) const;
		
		// -----------------------------------------------------------------------
		//  Private data members
		//
		//  fFromTable
		//      This is the 'from' table that we were given during construction.
		//      It is a 256 entry table of XMLCh chars. Each entry is the
		//      Unicode code point for the external encoding point of that value.
		//      So fFromTable[N] is the Unicode translation of code point N of
		//      the source encoding.
		//
		//      We don't own this table, we just refer to it. It is assumed that
		//      the table is static, for performance reasons.
		//
		//  fToSize
		//      The 'to' table is variable sized. This indicates how many records
		//      are in it.
		//
		//  fToTable
		//      This is a variable sized table of TransRec structures. It must
		//      be sorted by the intCh field, i.e. the XMLCh field. It is searched
		//      binarily to find the record for a particular Unicode char. Then
		//      that record's extch field is the translation record.
		//
		//      We don't own this table, we just refer to it. It is assumed that
		//      the table is static, for performance reasons.
		//
		//      NOTE: There may be dups of the extCh field, since there might be
		//      multiple Unicode code points which map to the same external code
		//      point. Normally this won't happen, since the parser assumes that
		//      internalization is normalized, but we have to be prepared to do
		//      the right thing if some client code gives us non-normalized data
		//      itself.
		// -----------------------------------------------------------------------
		const ICF_uint32*                        				fFromTable;
		long                           							fToSize;
		const Transcoder256Table::Transcoder256TableRecord*    	fToTable;



	};

} 
 }

#endif
