#include <mongoc.h>
#include <stdio.h>


int
main (int   argc,
      char *argv[])
{
   mongoc_client_t *client;
   mongoc_collection_t *collection;
   bson_error_t error;
   bson_t query;
   bson_t update;
   bson_t child;
   bson_t reply;
   char *str;

   mongoc_init ();

   client = mongoc_client_new ("mongodb://127.0.0.1:27017/");
   collection = mongoc_client_get_collection (client, "test", "test");

   /*
    * Build our query, {"cmpxchg": 1}
    */
   bson_init (&query);
   bson_append_int32 (&query, "cmpxchg", -1, 1);

   /*
    * Build our update. {"$set": {"cmpxchg": 2}}
    */
   bson_init (&update);
   bson_append_document_begin (&update, "$set", -1, &child);
   bson_append_int32 (&child, "cmpxchg", -1, 2);
   bson_append_document_end (&update, &child);

   /*
    * Submit the findAndModify.
    */
   if (!mongoc_collection_find_and_modify (collection, &query, NULL, &update, NULL, false, false, true, &reply, &error)) {
      fprintf (stderr, "find_and_modify() failure: %s\n", error.message);
      return 1;
   }

   /*
    * Print the result as JSON.
    */
   str = bson_as_json (&reply, NULL);
   printf ("%s\n", str);
   bson_free (str);

   /*
    * Cleanup.
    */
   bson_destroy (&query);
   bson_destroy (&update);
   bson_destroy (&reply);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);

   return 0;
}
