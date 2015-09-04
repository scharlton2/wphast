#ifndef XSTR_INCLUDED
#define XSTR_INCLUDED

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of char* data to XMLCh data.
// ---------------------------------------------------------------------------
class XStr
{
public :
	// -----------------------------------------------------------------------
	//  Constructors and Destructor
	// -----------------------------------------------------------------------
	XStr(const char* const toTranscode)
	{
		// Call the private transcoding method
		fUnicodeForm = xercesc::XMLString::transcode(toTranscode);
	}

	~XStr()
	{
		xercesc::XMLString::release(&fUnicodeForm);
	}


	// -----------------------------------------------------------------------
	//  Getter methods
	// -----------------------------------------------------------------------
	const XMLCh* unicodeForm() const
	{
		return fUnicodeForm;
	}

private :
	// -----------------------------------------------------------------------
	//  Private data members
	//
	//  fUnicodeForm
	//      This is the Unicode XMLCh format of the string.
	// -----------------------------------------------------------------------
	XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()

#endif // XSTR_INCLUDED