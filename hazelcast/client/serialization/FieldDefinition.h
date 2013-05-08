//
//  FieldDefinition.h
//  Server
//
//  Created by sancar koyunlu on 1/10/13.
//  Copyright (c) 2013 sancar koyunlu. All rights reserved.
//

#ifndef HAZELCAST_FIELD_DEFINITION
#define HAZELCAST_FIELD_DEFINITION

#include "DataSerializable.h"
#include "FieldType.h"
#include <iostream>
#include <string>

namespace hazelcast {
    namespace client {
        namespace serialization {


            class FieldDefinition : public DataSerializable {
            public:

                FieldDefinition();

                FieldDefinition(FieldDefinition const&);

                FieldDefinition(int, std::string, FieldType const&);

                FieldDefinition(int, std::string, FieldType const&, int, int);

                FieldType getType() const;

                std::string getName() const;

                int getIndex() const;

                int getFactoryId() const;

                int getClassId() const;

                void writeData(DataOutput&) const;

                void readData(DataInput&);

                bool operator ==(const FieldDefinition&) const;

                bool operator !=(const FieldDefinition&) const;


            private:
                int index;
                std::string fieldName;
                FieldType type;
                int classId;
                int factoryId;
            };

        }
    }
}
#endif /* HAZELCAST_FIELD_DEFINITION */