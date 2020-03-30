/*
    Database.hh - For connecting to a Firebase Realtime database 
    Created by Nick Leslie
*/

#ifndef Database_hh
#define Database_hh

//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h"

class Database {
    private:
        FirebaseData _firebaseData;
        String _nodeName;
        String _partnerNodeName;
        String _firebaseAuth;
        String _firebaseHost;
        bool _swState = false;
        String _path = "/node_state";

        void _setCredentials(String auth, String host);
        void _connectToDatabase();
        void _setNodeNames(String thisNode, String partnerNode);
        void _initDatabaseState();
        
    public:
        Database(String auth, String host, String thisNode, String partnerNode);
        bool setCurrentNodeRecord(bool state);
        bool getPartnerNodeRecord();
};

#endif
