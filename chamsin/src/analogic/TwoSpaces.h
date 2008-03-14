// **************************************************************
// TwoSpaces.h
// Author : Wu YiFei
// Date  : 12/04/2007 
// ***************************************************************


# ifndef HURRICANE_TWOSPACES
# define HURRICANE_TWOSPACES

# include  "Commons.h" 


BEGIN_NAMESPACE_HURRICANE

// ****************************************************************************************************
// TwoSpaces declaration.
// ****************************************************************************************************
class TwoSpaces {
// ***************

// Attributes
// **********
  private : string _s;
  private : unsigned _n;

// Constructors
// *************
  public : TwoSpaces(const string& s = "  ");
  public : TwoSpaces(const TwoSpaces&);

// Destructors
// ***********
  public : ~TwoSpaces(){};

// Operators
// *********
  public : TwoSpaces& operator=(const TwoSpaces&);
  public : TwoSpaces& operator++();
  public : TwoSpaces operator++(int);
  public : TwoSpaces& operator--();
  public : TwoSpaces operator--(int);
  public : TwoSpaces operator+(int);
  public : TwoSpaces operator-(int);

// Others
// ******
  public : string _GetString() const ;

};


// ****************************************************************************************************
// Variables declaration.
// ****************************************************************************************************
extern TwoSpaces ts;

END_NAMESPACE_HURRICANE


// ****************************************************************************************************
// Generic functions
// ****************************************************************************************************
string GetString(const H::TwoSpaces&);

inline ostream& operator<<(ostream& stream, const H::TwoSpaces& ts)
// ****************************************************************
{
  return stream<< GetString(ts);
}  

# endif    // END HURRICANE_TWOSPACES