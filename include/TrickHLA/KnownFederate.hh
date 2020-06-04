/*!
@file TrickHLA/KnownFederate.hh
@ingroup TrickHLA
@brief A class representing an HLA federate known to the Federation.

@copyright Copyright 2019 United States Government as represented by the
Administrator of the National Aeronautics and Space Administration.
No copyright is claimed in the United States under Title 17, U.S. Code.
All Other Rights Reserved.

\par<b>Responsible Organization</b>
Simulation and Graphics Branch, Mail Code ER7\n
Software, Robotics & Simulation Division\n
NASA, Johnson Space Center\n
2101 NASA Parkway, Houston, TX  77058

@trick_parse{everything}

@python_module{TrickHLA}

@revs_title
@revs_begin
@rev_entry{Dan Dexter, L3 Titan Group, DSES, April 2006, --, Initial implementation.}
@rev_entry{Dan Dexter, NASA ER7, TrickHLA, March 2019, --, Version 2 origin.}
@rev_entry{Edwin Z. Crues, NASA ER7, TrickHLA, March 2019, --, Version 3 rewrite.}
@revs_end

*/

#ifndef _TRICKHLA_KNOWN_FEDERATE_HH_
#define _TRICKHLA_KNOWN_FEDERATE_HH_

namespace TrickHLA
{

class KnownFederate
{
   // Let the Trick input processor access protected and private data.
   // InputProcessor is really just a marker class (does not really
   // exists - at least yet).  This friend statement just tells Trick
   // to go ahead and process the protected and private data as well
   // as the usual public data.
   friend class InputProcessor;
   // IMPORTANT Note: you must have the following line too.
   // Syntax: friend void init_attr<namespace>__<class name>();
   friend void init_attrTrickHLA__KnownFederate();

  public:
   char *MOM_instance_name; ///< @trick_units{--} MOM instance name for the federate object.

   //----------------------------- USER VARIABLES -----------------------------
   // Variables below this point are for either use within a users simulation
   // or must be configured by the user.

   char *name;     ///< @trick_units{--} Name of a Federate in the Federation.
   bool  required; ///< @trick_units{--} True requires federate to be in federation before continuing.

  public:
   /*! @brief Default constructor for the TrickHLA KnownFederate class. */
   KnownFederate() : MOM_instance_name( NULL ), name( NULL ), required( false ){};
   /*! @brief Destructor for the TrickHLA KnownFederate class. */
   ~KnownFederate(){};

  private:
   // Do not allow the copy constructor or assignment operator.
   /*! @brief Copy constructor for KnownFederate class.
    *  @details This constructor is private to prevent inadvertent copies. */
   KnownFederate( const KnownFederate &rhs );
   /*! @brief Assignment operator for KnownFederate class.
    *  @details This assignment operator is private to prevent inadvertent copies. */
   KnownFederate &operator=( const KnownFederate &rhs );
};

} // namespace TrickHLA

#endif // _TRICKHLA_KNOWN_FEDERATE_HH_
