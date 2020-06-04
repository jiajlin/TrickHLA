/*!
@file TrickHLA/Utilities.cpp
@ingroup TrickHLA
@brief Implementation of the TrickHLA utilities.

@copyright Copyright 2019 United States Government as represented by the
Administrator of the National Aeronautics and Space Administration.
No copyright is claimed in the United States under Title 17, U.S. Code.
All Other Rights Reserved.

\par<b>Responsible Organization</b>
Simulation and Graphics Branch, Mail Code ER7\n
Software, Robotics & Simulation Division\n
NASA, Johnson Space Center\n
2101 NASA Parkway, Houston, TX  77058

@tldh
@trick_link_dependency{Utilities.cpp}

@revs_title
@revs_begin
@rev_entry{Dan Dexter, L3 Titan Group, TrickHLA, Aug 2006, --, DSES TrickHLA Utilities.}
@rev_entry{Dan Dexter, NASA/ER7, TrickHLA, Sept 2010, --, Added Mac FPU control word support.}
@rev_entry{Edwin Z. Crues, NASA ER7, TrickHLA, March 2019, --, Version 3 rewrite.}
@revs_end

*/

// System include files.

// Trick include files.
#include "trick/trick_byteswap.h"

// TrickHLA include files.
#include "TrickHLA/Utilities.hh"
#include "TrickHLA/Version.hh"

// For a Mac, add support for the FPU control word value at program start.
#if defined( FPU_CW_PROTECTION ) && defined( __APPLE__ ) && ( defined( __i386__ ) || defined( __x86_64__ ) )
fpu_control_t __fpu_control;
#endif

using namespace std;
using namespace TrickHLA;

bool Utilities::is_transmission_byteswap(
   const EncodingEnum rti_encoding )
{
   const char endianness = Utilities::get_endianness();

   // Check encoding versus Endianness to determine if we need to byteswap.
   return ( ( ( rti_encoding == ENCODING_BIG_ENDIAN ) && ( endianness == TRICK_LITTLE_ENDIAN ) ) || ( ( rti_encoding == ENCODING_LITTLE_ENDIAN ) && ( endianness == TRICK_BIG_ENDIAN ) ) );
}

short Utilities::byteswap_short(
   short input )
{
   short          output;
   const size_t   size = sizeof( output );
   unsigned char *out  = (unsigned char *)&output;
   unsigned char *in   = (unsigned char *)&input;

   // sizeof(char) <= sizeof(short) <= sizeof(int)
   switch ( size ) {
   case 2:
      out[0] = in[1];
      out[1] = in[0];
      break;
   case 4:
      out[0] = in[3];
      out[1] = in[2];
      out[2] = in[1];
      out[3] = in[0];
      break;
   default:
      size_t i, k;
      for ( i = 0, k = size - 1; i < size; ++i, --k ) {
         out[i] = in[k];
      }
      break;
   }
   return ( output );
}

unsigned short Utilities::byteswap_unsigned_short(
   unsigned short input )
{
   unsigned short output;
   const size_t   size = sizeof( output );
   unsigned char *out  = (unsigned char *)&output;
   unsigned char *in   = (unsigned char *)&input;

   // sizeof(char) <= sizeof(short) <= sizeof(int)
   switch ( size ) {
   case 2:
      out[0] = in[1];
      out[1] = in[0];
      break;
   case 4:
      out[0] = in[3];
      out[1] = in[2];
      out[2] = in[1];
      out[3] = in[0];
      break;
   default:
      size_t i, k;
      for ( i = 0, k = size - 1; i < size; ++i, --k ) {
         out[i] = in[k];
      }
      break;
   }
   return ( output );
}

int Utilities::byteswap_int( // RETURN: -- Byteswap value.
   int input )               // IN: -- The input value to byteswap.
{
   int            output;
   const size_t   size = sizeof( output );
   unsigned char *out  = (unsigned char *)&output;
   unsigned char *in   = (unsigned char *)&input;

   // sizeof(short) <= sizeof(int) <= sizeof(long)
   switch ( size ) {
   case 4:
      out[0] = in[3];
      out[1] = in[2];
      out[2] = in[1];
      out[3] = in[0];
      break;
   default:
      size_t i, k;
      for ( i = 0, k = size - 1; i < size; ++i, --k ) {
         out[i] = in[k];
      }
      break;
   }
   return ( output );
}

unsigned int Utilities::byteswap_unsigned_int(
   unsigned int input )
{
   unsigned int   output;
   const size_t   size = sizeof( output );
   unsigned char *out  = (unsigned char *)&output;
   unsigned char *in   = (unsigned char *)&input;

   // sizeof(short) <= sizeof(int) <= sizeof(long)
   switch ( size ) {
   case 4:
      out[0] = in[3];
      out[1] = in[2];
      out[2] = in[1];
      out[3] = in[0];
      break;
   default:
      size_t i, k;
      for ( i = 0, k = size - 1; i < size; ++i, --k ) {
         out[i] = in[k];
      }
      break;
   }
   return ( output );
}

long Utilities::byteswap_long(
   long input )
{
   long           output;
   const size_t   size = sizeof( output );
   unsigned char *out  = (unsigned char *)&output;
   unsigned char *in   = (unsigned char *)&input;

   // sizeof(long) >= sizeof(int)
   switch ( size ) {
   case 4:
      out[0] = in[3];
      out[1] = in[2];
      out[2] = in[1];
      out[3] = in[0];
      break;
   case 8:
      out[0] = in[7];
      out[1] = in[6];
      out[2] = in[5];
      out[3] = in[4];
      out[4] = in[3];
      out[5] = in[2];
      out[6] = in[1];
      out[7] = in[0];
      break;
   default:
      size_t i, k;
      for ( i = 0, k = size - 1; i < size; ++i, --k ) {
         out[i] = in[k];
      }
      break;
   }
   return ( output );
}

unsigned long Utilities::byteswap_unsigned_long(
   unsigned long input )
{
   unsigned long  output;
   const size_t   size = sizeof( output );
   unsigned char *out  = (unsigned char *)&output;
   unsigned char *in   = (unsigned char *)&input;

   // sizeof(long) >= sizeof(int)
   switch ( size ) {
   case 4:
      out[0] = in[3];
      out[1] = in[2];
      out[2] = in[1];
      out[3] = in[0];
      break;
   case 8:
      out[0] = in[7];
      out[1] = in[6];
      out[2] = in[5];
      out[3] = in[4];
      out[4] = in[3];
      out[5] = in[2];
      out[6] = in[1];
      out[7] = in[0];
      break;
   default:
      size_t i, k;
      for ( i = 0, k = size - 1; i < size; ++i, --k ) {
         out[i] = in[k];
      }
      break;
   }
   return ( output );
}

long long Utilities::byteswap_long_long(
   long long input )
{
   long long      output;
   const size_t   size = sizeof( output );
   unsigned char *out  = (unsigned char *)&output;
   unsigned char *in   = (unsigned char *)&input;

   // Specified in the C99 standard, a long long is at least 64 bits.
   // sizeof(long long) >= sizeof(long)
   switch ( size ) {
   case 8:
      out[0] = in[7];
      out[1] = in[6];
      out[2] = in[5];
      out[3] = in[4];
      out[4] = in[3];
      out[5] = in[2];
      out[6] = in[1];
      out[7] = in[0];
      break;
   default:
      size_t i, k;
      for ( i = 0, k = size - 1; i < size; ++i, --k ) {
         out[i] = in[k];
      }
      break;
   }
   return ( output );
}

unsigned long long Utilities::byteswap_unsigned_long_long(
   unsigned long long input )
{
   unsigned long long output;
   const size_t       size = sizeof( output );
   unsigned char *    out  = (unsigned char *)&output;
   unsigned char *    in   = (unsigned char *)&input;

   // Specified in the C99 standard, a long long is at least 64 bits.
   // sizeof(long long) >= sizeof(long)
   switch ( size ) {
   case 8:
      out[0] = in[7];
      out[1] = in[6];
      out[2] = in[5];
      out[3] = in[4];
      out[4] = in[3];
      out[5] = in[2];
      out[6] = in[1];
      out[7] = in[0];
      break;
   default:
      size_t i, k;
      for ( i = 0, k = size - 1; i < size; ++i, --k ) {
         out[i] = in[k];
      }
      break;
   }
   return ( output );
}

float Utilities::byteswap_float(
   float input )
{
   float          output;
   const size_t   size = sizeof( output );
   unsigned char *out  = (unsigned char *)&output;
   unsigned char *in   = (unsigned char *)&input;

   switch ( size ) {
   case 4:
      out[0] = in[3];
      out[1] = in[2];
      out[2] = in[1];
      out[3] = in[0];
      break;
   case 8:
      out[0] = in[7];
      out[1] = in[6];
      out[2] = in[5];
      out[3] = in[4];
      out[4] = in[3];
      out[5] = in[2];
      out[6] = in[1];
      out[7] = in[0];
      break;
   default:
      size_t i, k;
      for ( i = 0, k = size - 1; i < size; ++i, --k ) {
         out[i] = in[k];
      }
      break;
   }
   return ( output );
}

double Utilities::byteswap_double(
   double input )
{
   double         output;
   const size_t   size = sizeof( output );
   unsigned char *out  = (unsigned char *)&output;
   unsigned char *in   = (unsigned char *)&input;

   switch ( size ) {
   case 8:
      out[0] = in[7];
      out[1] = in[6];
      out[2] = in[5];
      out[3] = in[4];
      out[4] = in[3];
      out[5] = in[2];
      out[6] = in[1];
      out[7] = in[0];
      break;
   default:
      size_t i, k;
      for ( i = 0, k = size - 1; i < size; ++i, --k ) {
         out[i] = in[k];
      }
      break;
   }
   return ( output );
}

string Utilities::get_version()
{
   return TRICKHLA_VERSION;
}

string Utilities::get_release_date()
{
   return TRICKHLA_RELEASE_DATE;
}
