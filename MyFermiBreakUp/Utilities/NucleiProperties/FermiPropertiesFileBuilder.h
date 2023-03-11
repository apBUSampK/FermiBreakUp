//
// Created by Artem Novikov on 11.03.2023.
//

#ifndef FERMIBREAKUP_MYFERMIBREAKUP_UTILITIES_NUCLEIPROPERTIES_FERMIPROPERTIESFILEBUILDER_H_
#define FERMIBREAKUP_MYFERMIBREAKUP_UTILITIES_NUCLEIPROPERTIES_FERMIPROPERTIESFILEBUILDER_H_

#include "VFermiPropertiesBuilder.h"

class FermiPropertiesFileBuilder : public VFermiPropertiesBuilder{
 public:
  void BuildTable(FermiNucleiProperties::MassMap& data) const override;
};

#endif //FERMIBREAKUP_MYFERMIBREAKUP_UTILITIES_NUCLEIPROPERTIES_FERMIPROPERTIESFILEBUILDER_H_
