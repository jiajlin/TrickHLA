/*!
@file TrickHLA/OwnershipHandler.cpp
@ingroup TrickHLA
@brief This class represents ownership transfer of HLA attributes for a
specific object.

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
@trick_link_dependency{Types.cpp}
@trick_link_dependency{Int64Time.cpp}
@trick_link_dependency{Int64Interval.cpp}
@trick_link_dependency{Attribute.cpp}
@trick_link_dependency{Object.cpp}
@trick_link_dependency{ExecutionControlBase.cpp}
@trick_link_dependency{OwnershipHandler.cpp}

@revs_title
@revs_begin
@rev_entry{Dan Dexter, L3 Titan Group, DSES, December 2006, --, Initial implementation.}
@rev_entry{Dan Dexter, NASA ER7, TrickHLA, March 2019, --, Version 2 origin.}
@rev_entry{Edwin Z. Crues, NASA ER7, TrickHLA, March 2019, --, Version 3 rewrite.}
@revs_end

*/

// System include files.
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

// Trick include files.
#include "trick/exec_proto.h"
#include "trick/memorymanager_c_intf.h"
#include "trick/message_proto.h" // for send_hs

// TrickHLA include files.
#include "TrickHLA/Attribute.hh"
#include "TrickHLA/Constants.hh"
#include "TrickHLA/ExecutionControlBase.hh"
#include "TrickHLA/Federate.hh"
#include "TrickHLA/Int64Interval.hh"
#include "TrickHLA/Object.hh"
#include "TrickHLA/OwnershipHandler.hh"
#include "TrickHLA/OwnershipItem.hh"

using namespace std;
using namespace TrickHLA;

/*!
 * @job_class{initialization}
 */
OwnershipHandler::OwnershipHandler()
   : object( NULL ),
     pull_requests(),
     push_requests(),
     pull_items_cnt( 0 ),
     pull_items( NULL ),
     push_items_cnt( 0 ),
     push_items( NULL )
{
   return;
}

/*!
 * @job_class{shutdown}
 */
OwnershipHandler::~OwnershipHandler()
{
   clear_checkpoint();
}

void OwnershipHandler::setup_checkpoint_requests()
{
   // Lock the ownership mutex since we are processing the ownership list.
   object->ownership_lock();

   // To keep from leaking memory make sure we clear all checkpointing
   // structures before we create new ones.
   clear_checkpoint();

   AttributeOwnershipMap::const_iterator owner_map_iter;
   THLAAttributeMap::const_iterator      attrib_iter;

   pull_items_cnt = 0;
   push_items_cnt = 0;

   // count the # of pull_request elements that need to be checkpointed
   if ( !pull_requests.empty() ) {

      // count the number of elements to allocate
      for ( owner_map_iter = pull_requests.begin(); owner_map_iter != pull_requests.end(); ++owner_map_iter ) {
         pull_items_cnt += owner_map_iter->second->size();
      }
   }

   // if there are any pull_request entries, encode them to get checkpointed.
   if ( pull_items_cnt > 0 ) {
      if ( should_print( DEBUG_LEVEL_3_TRACE, DEBUG_SOURCE_OWNERSHIP ) ) {
         send_hs( stdout, "OwnershipHandler::setup_checkpoint_requests():%d Checkpointing %d pull_request elements.%c",
                  __LINE__, pull_items_cnt, THLA_NEWLINE );
      }
      pull_items = reinterpret_cast< OwnershipItem * >( alloc_type( (int)pull_items_cnt, "TrickHLA::OwnershipItem" ) );
      if ( pull_items == static_cast< OwnershipItem * >( NULL ) ) {
         send_hs( stderr, "OwnershipHandler::setup_checkpoint_requests():%d \
Could not allocate memory for pull_items (array of OwnershipItem type)!%c",
                  __LINE__, THLA_NEWLINE );
         exec_terminate( __FILE__, "OwnershipHandler::setup_checkpoint_requests(): \
Could not allocate memory for pull_items (array of OwnershipItem type)!" );
         return;
      }

      // now, encode them to get checkpointed.
      int count = 0;
      for ( owner_map_iter = pull_requests.begin(); owner_map_iter != pull_requests.end(); ++owner_map_iter ) {
         double            curr_time = owner_map_iter->first;
         THLAAttributeMap *tMap      = owner_map_iter->second;
         for ( attrib_iter = tMap->begin(); attrib_iter != tMap->end(); ++attrib_iter ) {
            pull_items[count].time     = curr_time;
            pull_items[count].FOM_name = TMM_strdup( (char *)attrib_iter->first.c_str() );
            count++;
         }
      }
   }

   // Count the # of push_request elements that need to be checkpointed
   if ( !push_requests.empty() ) {

      // count the number of elements to allocate
      for ( owner_map_iter = push_requests.begin(); owner_map_iter != push_requests.end(); ++owner_map_iter ) {
         push_items_cnt += owner_map_iter->second->size();
      }
   }

   // if there are any push_request entries, encode them to get checkpointed.
   if ( push_items_cnt > 0 ) {
      if ( should_print( DEBUG_LEVEL_3_TRACE, DEBUG_SOURCE_OWNERSHIP ) ) {
         send_hs( stdout, "OwnershipHandler::setup_checkpoint_requests():%d Checkpointing %d push_request elements.%c",
                  __LINE__, push_items_cnt, THLA_NEWLINE );
      }
      push_items = reinterpret_cast< OwnershipItem * >( alloc_type( (int)push_items_cnt, "TrickHLA::OwnershipItem" ) );
      if ( push_items == static_cast< OwnershipItem * >( NULL ) ) {
         send_hs( stderr, "OwnershipHandler::setup_checkpoint_requests():%d \
Could not allocate memory for push_items (array of OwnershipItem type)!%c",
                  __LINE__, THLA_NEWLINE );
         exec_terminate( __FILE__, "OwnershipHandler::setup_checkpoint_requests(): \
Could not allocate memory for push_items (array of OwnershipItem type)!" );
         return;
      }

      // now, encode them to get checkpointed.
      int count = 0;
      for ( owner_map_iter = push_requests.begin(); owner_map_iter != push_requests.end(); ++owner_map_iter ) {
         double            curr_time = owner_map_iter->first;
         THLAAttributeMap *tMap      = owner_map_iter->second;
         for ( attrib_iter = tMap->begin(); attrib_iter != tMap->end(); ++attrib_iter ) {
            push_items[count].time     = curr_time;
            push_items[count].FOM_name = TMM_strdup( (char *)attrib_iter->first.c_str() );
            count++;
         }
      }
   }

   // Unlock the onwership mutex now that we are done with the list.
   object->ownership_unlock();
}

void OwnershipHandler::clear_checkpoint()
{
   // If there are any pull_request entries, delete them
   if ( pull_items_cnt > 0 ) {
      for ( size_t i = 0; i < pull_items_cnt; i++ ) {
         pull_items[i].clear();
      }
      TMM_delete_var_a( pull_items );
      pull_items     = NULL;
      pull_items_cnt = 0;
   }

   // if there are any push_request entries, delete them
   if ( push_items_cnt > 0 ) {
      for ( size_t i = 0; i < push_items_cnt; i++ ) {
         push_items[i].clear();
      }
      TMM_delete_var_a( push_items );
      push_items     = NULL;
      push_items_cnt = 0;
   }
}

void OwnershipHandler::restore_requests()
{
   THLAAttributeMap *                    attr_map;
   AttributeOwnershipMap::const_iterator ownership_iter;

   // Lock the ownership mutex since we are processing the ownership list.
   object->ownership_lock();

   // Decode all the ownership-items in the pull_items.
   if ( pull_items_cnt > 0 ) {

      for ( size_t count = 0; count < pull_items_cnt; count++ ) {

         double time    = pull_items[count].time;
         ownership_iter = pull_requests.find( time );

         // Determine if there is an existing attribute map for the time. If not, add one.
         if ( ownership_iter == pull_requests.end() ) {
            attr_map            = new THLAAttributeMap();
            pull_requests[time] = attr_map;
         } else {
            attr_map = ownership_iter->second;
         }

         // get the attribute from the Object
         Attribute *attribute = get_attribute( pull_items[count].FOM_name );

         // Add the attribute to the map, using the FOM_name as the key.
         attr_map->insert( make_pair( pull_items[count].FOM_name, attribute ) );

         if ( should_print( DEBUG_LEVEL_3_TRACE, DEBUG_SOURCE_OWNERSHIP ) ) {
            send_hs( stdout, "OwnershipHandler::restore_requests():%d Restoring ownership pull item attribute \"%s\"%c",
                     __LINE__, pull_items[count].FOM_name, THLA_NEWLINE );
         }
      }
   }

   // Decode all the ownership-items in the push_items.
   if ( push_items_cnt > 0 ) {

      for ( size_t count = 0; count < push_items_cnt; count++ ) {

         double time    = push_items[count].time;
         ownership_iter = push_requests.find( time );

         // Determine if there is an existing attribute map for the time. If not, add one.
         if ( ownership_iter == push_requests.end() ) {
            attr_map            = new THLAAttributeMap();
            push_requests[time] = attr_map;
         } else {
            attr_map = ownership_iter->second;
         }

         // get the attribute from the Object
         Attribute *attribute = get_attribute( push_items[count].FOM_name );

         // Add the attribute to the map, using the FOM_name as the key.
         attr_map->insert( make_pair( push_items[count].FOM_name, attribute ) );

         if ( should_print( DEBUG_LEVEL_3_TRACE, DEBUG_SOURCE_OWNERSHIP ) ) {
            send_hs( stdout, "OwnershipHandler::restore_requests():%d Restoring ownership push item attribute \"%s\"%c",
                     __LINE__, push_items[count].FOM_name, THLA_NEWLINE );
         }
      }
   }

   // Unlock the onwership mutex now that we are done with the list.
   object->ownership_unlock();
}

void OwnershipHandler::initialize_callback(
   Object *obj )
{
   this->object = obj;
}

bool OwnershipHandler::should_print(
   const DebugLevelEnum & level,
   const DebugSourceEnum &code ) const
{
   if ( object != NULL ) {
      return object->should_print( level, code );
   }
   return true;
}

string OwnershipHandler::get_object_name() const
{
   return ( ( this->object != NULL ) ? string( object->get_name() ) : string( "" ) );
}

string OwnershipHandler::get_object_FOM_name() const
{
   return ( ( this->object != NULL ) ? string( object->get_FOM_name() ) : string( "" ) );
}

int OwnershipHandler::get_attribute_count() const
{
   return ( ( this->object != NULL ) ? object->get_attribute_count() : 0 );
}

VectorOfStrings OwnershipHandler::get_attribute_FOM_names() const
{
   return ( ( this->object != NULL ) ? object->get_attribute_FOM_names() : VectorOfStrings() );
}

Attribute *OwnershipHandler::get_attribute(
   const char *attribute_FOM_name )
{
   return ( ( object != NULL ) ? object->get_attribute( attribute_FOM_name ) : NULL );
}

bool OwnershipHandler::is_locally_owned(
   const char *attribute_FOM_name )
{
   Attribute *attribute = get_attribute( attribute_FOM_name );
   return ( ( attribute != NULL ) ? attribute->is_locally_owned() : false );
}

bool OwnershipHandler::is_remotely_owned(
   const char *attribute_FOM_name )
{
   Attribute *attribute = get_attribute( attribute_FOM_name );
   return ( ( attribute != NULL ) ? attribute->is_remotely_owned() : false );
}

bool OwnershipHandler::is_published(
   const char *attribute_FOM_name )
{
   Attribute *attribute = get_attribute( attribute_FOM_name );
   return ( ( attribute != NULL ) ? attribute->is_publish() : false );
}

bool OwnershipHandler::is_subscribed(
   const char *attribute_FOM_name )
{
   Attribute *attribute = get_attribute( attribute_FOM_name );
   return ( ( attribute != NULL ) ? attribute->is_subscribe() : false );
}

void OwnershipHandler::pull_ownership()
{
   pull_ownership( -std::numeric_limits< double >::max() );
}

void OwnershipHandler::pull_ownership(
   double time )
{
   if ( this->object == NULL ) {
      return;
   }

   if ( should_print( DEBUG_LEVEL_3_TRACE, DEBUG_SOURCE_OWNERSHIP ) ) {
      send_hs( stdout, "OwnershipHandler::pull_ownership(time=%G):%d scenario-time=%G, granted_time=%G, lookahead=%G %c",
               time, __LINE__, get_scenario_time(),
               get_granted_fed_time().get_double_time(),
               get_fed_lookahead().get_double_time(), THLA_NEWLINE );
   }

   THLAAttributeMap *attr_map;

   // Lock the ownership mutex since we are processing the ownership pull list.
   object->ownership_lock();

   // Find the attribute map for the specified time.
   AttributeOwnershipMap::const_iterator attr_map_iter = pull_requests.find( time );

   // Determine if there is an existing attribute map for the time, if not add one.
   if ( attr_map_iter == pull_requests.end() ) {
      attr_map            = new THLAAttributeMap();
      pull_requests[time] = attr_map;
   } else {
      attr_map = attr_map_iter->second;
   }

   int        attr_cnt   = object->get_attribute_count();
   Attribute *attributes = object->get_attributes();

   // The user has requested to pull the ownership of all the attributes.
   for ( int i = 0; i < attr_cnt; i++ ) {

      // Use the Attribute FOM name as the key instead of the AttributeHandle
      // because the handle may not have been initialized yet.
      string key( attributes[i].get_FOM_name() );

      // Add the attribute to the map.
      attr_map->insert( make_pair( key, &attributes[i] ) );
   }

   // Unlock the ownership mutex now that we are done with the pull list.
   object->ownership_unlock();
}

void OwnershipHandler::pull_ownership( // RETURN: -- None.
   const char *attribute_FOM_name )    // IN: -- Attribute FOM name.
{
   pull_ownership( attribute_FOM_name, -std::numeric_limits< double >::max() );
}

void OwnershipHandler::pull_ownership(
   const char *attribute_FOM_name,
   double      time )
{
   // Find the attribute for the givn attribute FOM name.
   Attribute *attribute = get_attribute( attribute_FOM_name );

   // Just return if the attribute was not found.
   if ( attribute == NULL ) {
      return;
   }

   if ( should_print( DEBUG_LEVEL_3_TRACE, DEBUG_SOURCE_OWNERSHIP ) ) {
      send_hs( stdout, "OwnershipHandler::pull_ownership(%s, time=%G):%d scenario-time=%G, granted_time=%G, lookahead=%G %c",
               attribute_FOM_name, time, __LINE__, get_scenario_time(),
               get_granted_fed_time().get_double_time(),
               get_fed_lookahead().get_double_time(), THLA_NEWLINE );
   }

   THLAAttributeMap *attr_map;

   // Lock the ownership mutex since we are processing the ownership pull list.
   object->ownership_lock();

   // Find the attribute map for the specified time.
   AttributeOwnershipMap::const_iterator attr_map_iter = pull_requests.find( time );

   // Determine if there is an existing attribute map for the time, if not add one.
   if ( attr_map_iter == pull_requests.end() ) {
      attr_map            = new THLAAttributeMap();
      pull_requests[time] = attr_map;
   } else {
      attr_map = attr_map_iter->second;
   }

   // Use the Attribute FOM name as the key instead of the AttributeHandle
   // because it may not have been initialized yet.
   string key( attribute->get_FOM_name() );

   // Add the attribute to the map.
   attr_map->insert( make_pair( key, attribute ) );

   // Unlock the ownership mutex now that we are done with the pull list.
   object->ownership_unlock();
}

void OwnershipHandler::push_ownership()
{
   push_ownership( -std::numeric_limits< double >::max() );
}

void OwnershipHandler::push_ownership(
   double time )
{
   if ( this->object == NULL ) {
      return;
   }

   if ( should_print( DEBUG_LEVEL_3_TRACE, DEBUG_SOURCE_OWNERSHIP ) ) {
      send_hs( stdout, "OwnershipHandler::push_ownership(time=%G):%d sim-time=%G, granted_time=%G, lookahead=%G %c",
               time, __LINE__, get_scenario_time(),
               get_granted_fed_time().get_double_time(),
               get_fed_lookahead().get_double_time(), THLA_NEWLINE );
   }

   THLAAttributeMap *attr_map;

   // Lock the ownership mutex since we are processing the ownership push list.
   object->ownership_lock();

   // Find the attribute map for the specified time.
   AttributeOwnershipMap::const_iterator attr_map_iter = push_requests.find( time );

   // Determine if there is an existing attribute map for the time, if not add one.
   if ( attr_map_iter == push_requests.end() ) {
      attr_map            = new THLAAttributeMap();
      push_requests[time] = attr_map;
   } else {
      attr_map = attr_map_iter->second;
   }

   int        attr_cnt   = object->get_attribute_count();
   Attribute *attributes = object->get_attributes();

   // The user has requested to push the ownership of all the attributes.
   for ( int i = 0; i < attr_cnt; i++ ) {

      // Use the Attribute FOM name as the key instead of the AttribyteHandle
      // because it may not have been initialized yet.
      string key( attributes[i].get_FOM_name() );

      // Add the attribute to the map.
      attr_map->insert( make_pair( key, &attributes[i] ) );
   }

   // Unlock the onwership mutex now that we are done with the push list.
   object->ownership_unlock();
}

void OwnershipHandler::push_ownership(
   const char *attribute_FOM_name )
{
   push_ownership( attribute_FOM_name, -std::numeric_limits< double >::max() );
}

void OwnershipHandler::push_ownership(
   const char *attribute_FOM_name,
   double      time )
{
   // Find the attribute for the given attribute FOM name.
   Attribute *attribute = get_attribute( attribute_FOM_name );

   // Just return if the attribute was not found.
   if ( attribute == NULL ) {
      return;
   }

   if ( should_print( DEBUG_LEVEL_3_TRACE, DEBUG_SOURCE_OWNERSHIP ) ) {
      send_hs( stdout, "OwnershipHandler::push_ownership(%s, time=%G):%d sim-time=%G, granted_time=%G, lookahead=%G %c",
               attribute_FOM_name, time, __LINE__, get_scenario_time(),
               get_granted_fed_time().get_double_time(),
               get_fed_lookahead().get_double_time(), THLA_NEWLINE );
   }

   THLAAttributeMap *attr_map;

   // Lock the ownership mutex since we are processing the ownership push list.
   object->ownership_lock();

   // Find the attribute map for the specified time.
   AttributeOwnershipMap::const_iterator attr_map_iter = push_requests.find( time );

   // Determine if there is an existing attribute map for the time, if not add one.
   if ( attr_map_iter == push_requests.end() ) {
      attr_map            = new THLAAttributeMap();
      push_requests[time] = attr_map;
   } else {
      attr_map = attr_map_iter->second;
   }

   // Use the Attribute FOM name as the key instead of the AttributeHandle
   // because it may not have been initialized yet.
   string key( attribute->get_FOM_name() );

   // Add the attribute to the map.
   attr_map->insert( make_pair( key, attribute ) );

   // Unlock the ownership mutex now that we are done with the push list.
   object->ownership_unlock();
}

/*!
 *  @details If the manager does not exist, -1.0 seconds is assigned to the returned object.
 */
Int64Interval OwnershipHandler::get_fed_lookahead() const
{
   Int64Interval di;
   if ( object != NULL ) {
      di = object->get_fed_lookahead();
   } else {
      di = Int64Interval( -1.0 );
   }
   return di;
}

/*!
 *  @details If the object does not exist, MAX_LOGICAL_TIME_SECONDS is
 *  assigned to the returned object. */
Int64Time OwnershipHandler::get_granted_fed_time() const
{
   Int64Time dt;
   if ( object != NULL ) {
      dt = object->get_granted_fed_time();
   } else {
      dt = Int64Time( MAX_LOGICAL_TIME_SECONDS );
   }
   return dt;
}

double OwnershipHandler::get_scenario_time()
{
   if ( ( object != NULL ) && ( object->get_federate() != NULL ) ) {
      ExecutionControlBase *execution_control = object->get_federate()->get_execution_control();
      return ( execution_control->get_scenario_time() );
   }
   return -std::numeric_limits< double >::max();
}

double OwnershipHandler::get_cte_time()
{
   if ( ( object != NULL ) && ( object->get_federate() != NULL ) ) {
      ExecutionControlBase *execution_control = object->get_federate()->get_execution_control();
      if ( execution_control->does_cte_timeline_exist() ) {
         return execution_control->get_cte_time();
      }
   }
   return -std::numeric_limits< double >::max();
}
