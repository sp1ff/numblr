#include <cstring>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <ext/codecvt_specializations.h>
#include <ext/stdio_filebuf.h>
#include <ext/stdio_sync_filebuf.h>
#include <bits/c++io.h>  // For __c_file
#include <bits/move.h>   // For __exchange
using namespace std;
using namespace __gnu_cxx;
// An encoding_state whose internal representation is Windows-1252 & whose
// external is UTF-8
struct my_encoding_state: public encoding_state {
  my_encoding_state(): encoding_state("WINDOWS-1252", "UTF-8")
  { }
};
namespace std {
  template<typename _InternT, typename _ExternT>
  class codecvt<_InternT, _ExternT, my_encoding_state>
    : public __codecvt_abstract_base<_InternT, _ExternT, my_encoding_state>
  {
  public:      
    // Types:
    typedef codecvt_base::result			result;
    typedef _InternT 					intern_type;
    typedef _ExternT 					extern_type;
    typedef my_encoding_state 		state_type;
    typedef state_type::descriptor_type 		descriptor_type;

    // Data Members:
    static locale::id 		id;

    explicit 
    codecvt(size_t __refs = 0)
      : __codecvt_abstract_base<intern_type, extern_type, state_type>(__refs)
    { }

    explicit 
    codecvt(state_type& __enc, size_t __refs = 0)
      : __codecvt_abstract_base<intern_type, extern_type, state_type>(__refs)
    { }

  protected:
    virtual 
    ~codecvt() { }

    virtual result
    do_out(state_type& __state, const intern_type* __from, 
	   const intern_type* __from_end, const intern_type*& __from_next,
	   extern_type* __to, extern_type* __to_end,
	   extern_type*& __to_next) const;

    virtual result
    do_unshift(state_type& __state, extern_type* __to, 
	       extern_type* __to_end, extern_type*& __to_next) const;

    virtual result
    do_in(state_type& __state, const extern_type* __from, 
	  const extern_type* __from_end, const extern_type*& __from_next,
	  intern_type* __to, intern_type* __to_end, 
	  intern_type*& __to_next) const;

    virtual int 
    do_encoding() const throw();

    virtual bool 
    do_always_noconv() const throw();

    virtual int 
    do_length(state_type&, const extern_type* __from, 
	      const extern_type* __end, size_t __max) const;

    virtual int 
    do_max_length() const throw();
  };

  template<typename _InternT, typename _ExternT>
  locale::id 
  codecvt<_InternT, _ExternT, my_encoding_state>::id;

  template<typename _InternT, typename _ExternT>
  codecvt_base::result
  codecvt<_InternT, _ExternT, my_encoding_state>::
  do_out(state_type& __state, const intern_type* __from, 
	 const intern_type* __from_end, const intern_type*& __from_next,
	 extern_type* __to, extern_type* __to_end,
	 extern_type*& __to_next) const
  {
    result __ret = codecvt_base::error;
    if (__state.good())
      {
	const descriptor_type& __desc = __state.out_descriptor();
	const size_t __fmultiple = sizeof(intern_type);
	size_t __fbytes = __fmultiple * (__from_end - __from);
	const size_t __tmultiple = sizeof(extern_type);
	size_t __tbytes = __tmultiple * (__to_end - __to); 
	  
	// Argument list for iconv specifies a byte sequence. Thus,
	// all to/from arrays must be brutally casted to char*.
	char* __cto = reinterpret_cast<char*>(__to);
	char* __cfrom;
	size_t __conv;

	// Some encodings need a byte order marker as the first item
	// in the byte stream, to designate endian-ness. The default
	// value for the byte order marker is NULL, so if this is
	// the case, it's not necessary and we can just go on our
	// merry way.
	int __int_bom = __state.internal_bom();
	if (__int_bom)
	  {	  
	    size_t __size = __from_end - __from;
	    intern_type* __cfixed = static_cast<intern_type*>
	      (__builtin_alloca(sizeof(intern_type) * (__size + 1)));
	    __cfixed[0] = static_cast<intern_type>(__int_bom);
	    char_traits<intern_type>::copy(__cfixed + 1, __from, __size);
	    __cfrom = reinterpret_cast<char*>(__cfixed);
	    __conv = __iconv_adaptor(iconv, __desc, &__cfrom,
				     &__fbytes, &__cto, &__tbytes); 
	  }
	else
	  {
	    intern_type* __cfixed = const_cast<intern_type*>(__from);
	    __cfrom = reinterpret_cast<char*>(__cfixed);
	    __conv = __iconv_adaptor(iconv, __desc, &__cfrom, &__fbytes, 
				     &__cto, &__tbytes); 
	  }

	if (__conv != size_t(-1))
	  {
	    __from_next = reinterpret_cast<const intern_type*>(__cfrom);
	    __to_next = reinterpret_cast<extern_type*>(__cto);
	    __ret = codecvt_base::ok;
	  }
	else 
	  {
	    if (__fbytes < __fmultiple * (__from_end - __from))
	      {
		__from_next = reinterpret_cast<const intern_type*>(__cfrom);
		__to_next = reinterpret_cast<extern_type*>(__cto);
		__ret = codecvt_base::partial;
	      }
	    else
	      __ret = codecvt_base::error;
	  }
      }
    return __ret; 
  }

  template<typename _InternT, typename _ExternT>
  codecvt_base::result
  codecvt<_InternT, _ExternT, my_encoding_state>::
  do_unshift(state_type& __state, extern_type* __to, 
	     extern_type* __to_end, extern_type*& __to_next) const
  {
    result __ret = codecvt_base::error;
    if (__state.good())
      {
	const descriptor_type& __desc = __state.in_descriptor();
	const size_t __tmultiple = sizeof(intern_type);
	size_t __tlen = __tmultiple * (__to_end - __to); 
	  
	// Argument list for iconv specifies a byte sequence. Thus,
	// all to/from arrays must be brutally casted to char*.
	char* __cto = reinterpret_cast<char*>(__to);
	size_t __conv = __iconv_adaptor(iconv,__desc, 0, 0,
					&__cto, &__tlen); 
	  
	if (__conv != size_t(-1))
	  {
	    __to_next = reinterpret_cast<extern_type*>(__cto);
	    if (__tlen == __tmultiple * (__to_end - __to))
	      __ret = codecvt_base::noconv;
	    else if (__tlen == 0)
	      __ret = codecvt_base::ok;
	    else
	      __ret = codecvt_base::partial;
	  }
	else 
	  __ret = codecvt_base::error;
      }
    return __ret; 
  }
   
  template<typename _InternT, typename _ExternT>
  codecvt_base::result
  codecvt<_InternT, _ExternT, my_encoding_state>::
  do_in(state_type& __state, const extern_type* __from, 
	const extern_type* __from_end, const extern_type*& __from_next,
	intern_type* __to, intern_type* __to_end, 
	intern_type*& __to_next) const
  { 
    result __ret = codecvt_base::error;
    if (__state.good())
      {
	const descriptor_type& __desc = __state.in_descriptor();
	const size_t __fmultiple = sizeof(extern_type);
	size_t __flen = __fmultiple * (__from_end - __from);
	const size_t __tmultiple = sizeof(intern_type);
	size_t __tlen = __tmultiple * (__to_end - __to); 
	  
	// Argument list for iconv specifies a byte sequence. Thus,
	// all to/from arrays must be brutally casted to char*.
	char* __cto = reinterpret_cast<char*>(__to);
	char* __cfrom;
	size_t __conv;

	// Some encodings need a byte order marker as the first item
	// in the byte stream, to designate endian-ness. The default
	// value for the byte order marker is NULL, so if this is
	// the case, it's not necessary and we can just go on our
	// merry way.
	int __ext_bom = __state.external_bom();
	if (__ext_bom)
	  {	  
	    size_t __size = __from_end - __from;
	    extern_type* __cfixed =  static_cast<extern_type*>
	      (__builtin_alloca(sizeof(extern_type) * (__size + 1)));
	    __cfixed[0] = static_cast<extern_type>(__ext_bom);
	    char_traits<extern_type>::copy(__cfixed + 1, __from, __size);
	    __cfrom = reinterpret_cast<char*>(__cfixed);
	    __conv = __iconv_adaptor(iconv, __desc, &__cfrom,
				     &__flen, &__cto, &__tlen); 
	  }
	else
	  {
	    extern_type* __cfixed = const_cast<extern_type*>(__from);
	    __cfrom = reinterpret_cast<char*>(__cfixed);
	    __conv = __iconv_adaptor(iconv, __desc, &__cfrom,
				     &__flen, &__cto, &__tlen); 
	  }

	  
	if (__conv != size_t(-1))
	  {
	    __from_next = reinterpret_cast<const extern_type*>(__cfrom);
	    __to_next = reinterpret_cast<intern_type*>(__cto);
	    __ret = codecvt_base::ok;
	  }
	else 
	  {
	    if (__flen < static_cast<size_t>(__from_end - __from))
	      {
		__from_next = reinterpret_cast<const extern_type*>(__cfrom);
		__to_next = reinterpret_cast<intern_type*>(__cto);
		__ret = codecvt_base::partial;
	      }
	    else
	      __ret = codecvt_base::error;
	  }
      }
    return __ret; 
  }
  
  template<typename _InternT, typename _ExternT>
  int 
  codecvt<_InternT, _ExternT, my_encoding_state>::
  do_encoding() const throw()
  {
    int __ret = 0;
    if (sizeof(_ExternT) <= sizeof(_InternT))
      __ret = sizeof(_InternT) / sizeof(_ExternT);
    return __ret; 
  }
  
  template<typename _InternT, typename _ExternT>
  bool 
  codecvt<_InternT, _ExternT, my_encoding_state>::
  do_always_noconv() const throw()
  { return false; }
  
  template<typename _InternT, typename _ExternT>
  int 
  codecvt<_InternT, _ExternT, my_encoding_state>::
  do_length(state_type&, const extern_type* __from, 
	    const extern_type* __end, size_t __max) const
  { return std::min(__max, static_cast<size_t>(__end - __from)); }

  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // 74.  Garbled text for codecvt::do_max_length
  template<typename _InternT, typename _ExternT>
  int 
  codecvt<_InternT, _ExternT, my_encoding_state>::
  do_max_length() const throw()
  { return 4; }
}
struct my_traits
{
  typedef my_encoding_state                               state_type;
  typedef char                                            char_type;
  typedef typename __gnu_cxx::_Char_types<char>::int_type int_type;
  typedef typename std::fpos<my_encoding_state>           pos_type;
  typedef typename __gnu_cxx::_Char_types<char>::off_type off_type;

  static void
  assign(char_type& __c1, const char_type& __c2)
  { __c1 = __c2; }

  static _GLIBCXX_CONSTEXPR bool
  eq(const char_type& __c1, const char_type& __c2)
  { return __c1 == __c2; }

  static _GLIBCXX_CONSTEXPR bool
  lt(const char_type& __c1, const char_type& __c2)
  { return __c1 < __c2; }

  static int
  compare(const char_type* __s1, const char_type* __s2, std::size_t __n);

  static std::size_t
  length(const char_type* __s);

  static const char_type*
  find(const char_type* __s, std::size_t __n, const char_type& __a);

  static char_type*
  move(char_type* __s1, const char_type* __s2, std::size_t __n);

  static char_type*
  copy(char_type* __s1, const char_type* __s2, std::size_t __n);

  static char_type*
  assign(char_type* __s, std::size_t __n, char_type __a);

  static _GLIBCXX_CONSTEXPR char_type
  to_char_type(const int_type& __c)
  { return static_cast<char_type>(__c); }

  static _GLIBCXX_CONSTEXPR int_type
  to_int_type(const char_type& __c)
  { return static_cast<int_type>(__c); }

  static _GLIBCXX_CONSTEXPR bool
  eq_int_type(const int_type& __c1, const int_type& __c2)
  { return __c1 == __c2; }

  static _GLIBCXX_CONSTEXPR int_type
  eof()
  { return static_cast<int_type>(_GLIBCXX_STDIO_EOF); }

  static _GLIBCXX_CONSTEXPR int_type
  not_eof(const int_type& __c)
  { return !eq_int_type(__c, eof()) ? __c : to_int_type(char_type()); }
};

int
my_traits::
compare(const char_type* __s1, const char_type* __s2, std::size_t __n)
{
  for (std::size_t __i = 0; __i < __n; ++__i)
    if (lt(__s1[__i], __s2[__i]))
      return -1;
    else if (lt(__s2[__i], __s1[__i]))
      return 1;
  return 0;
}

std::size_t
my_traits::
length(const char_type* __p)
{
  std::size_t __i = 0;
  while (!eq(__p[__i], char_type()))
    ++__i;
  return __i;
}

const typename my_traits::char_type*
my_traits::
find(const char_type* __s, std::size_t __n, const char_type& __a)
{
  for (std::size_t __i = 0; __i < __n; ++__i)
    if (eq(__s[__i], __a))
      return __s + __i;
  return 0;
}

typename my_traits::char_type*
my_traits::
move(char_type* __s1, const char_type* __s2, std::size_t __n)
{
  return static_cast<char*>(__builtin_memmove(__s1, __s2,
						__n * sizeof(char_type)));
}

typename my_traits::char_type*
my_traits::
copy(char_type* __s1, const char_type* __s2, std::size_t __n)
{
  // NB: Inline std::copy so no recursive dependencies.
  std::copy(__s2, __s2 + __n, __s1);
  return __s1;
}


typename my_traits::char_type*
my_traits::
assign(char_type* __s, std::size_t __n, char_type __a)
{
  // NB: Inline std::fill_n so no recursive dependencies.
  std::fill_n(__s, __n, __a);
  return __s;
}

int main() {
  const char text[] = "1€"; // 1(euro) in Windows-1252
  cout << "              bytes:";
  for (size_t i = 0, n = sizeof(text); i < n; ++i) {
    cout << " " << hex << setw(2) << setfill('0') << 
      static_cast<unsigned int>(static_cast<unsigned char>(text[i]));
  }
  cout << endl;

  locale loc(locale(""), new codecvt<char, char, my_encoding_state>());

  typedef codecvt<char, char, my_encoding_state> my_codecvt;

  const my_codecvt &cvt = use_facet<my_codecvt>(loc);

  char out[5] = { 0 };
  char *outn;
  const char *fromn;
  my_encoding_state state;
  codecvt_base::result r = cvt.out(state, text, text + sizeof(text), 
				   fromn,
				   out, out + 5, 
				   outn);
  cout << "    codecvt_base status: " << r << endl;
  cout << "                  bytes:";
  for (size_t i = 0, n = sizeof(out); i < n; ++i) {
    cout << " " << hex << setw(2) << setfill('0') << 
      static_cast<unsigned int>(static_cast<unsigned char>(out[i]));
  }
  cout << endl;

  locale::global(loc);
  cout << "     global locale name: " << locale().name() << endl;
  cout << "    has_facet<locale()>: " << has_facet<my_codecvt>(locale()) << endl;

  basic_ofstream<char, my_traits> ofs("6.txt");
  cout << "has_facet<ofs.getloc()>: " << has_facet<my_codecvt>(ofs.getloc()) << endl;
  ofs << text << endl;

  return 0;
}
