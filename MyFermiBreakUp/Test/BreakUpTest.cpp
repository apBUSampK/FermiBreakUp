//
// Created by Artem Novikov on 19.03.2023.
//

#include <gtest/gtest.h>
#include <CLHEP/Units/PhysicalConstants.h>

#include "FermiBreakUp.h"
#include "NucleiProperties/NucleiProperties.h"
#include "Randomizer.h"

float CalculateFragmentCount(MassNumber mass, ChargeNumber charge, FermiFloat energy, size_t tests) {
  size_t parts_counter = 0;
  auto additional_energy = energy * FermiFloat(mass);
  auto vec = LorentzVector(0, 0, 0, NucleiProperties().GetNuclearMass(mass, charge) + additional_energy);
  auto particle = FermiParticle(mass, charge, vec);
  for (size_t i = 0; i < tests; ++i) {
    auto particles = FermiBreakUp::BreakItUp(particle);
    parts_counter += particles.size();
  }
  return float(parts_counter) / tests;
}

TEST(FermiBreakUpTests, CarbonTest) {
  size_t runs = 1e3;
  /// Carbons shouldn't break up [0, 0.7]
  ASSERT_NEAR(1, CalculateFragmentCount(12_m, 6_c, 0 * CLHEP::MeV, runs), 0.01);
  ASSERT_NEAR(1, CalculateFragmentCount(12_m, 6_c, 0.25 * CLHEP::MeV, runs), 0.01);
  ASSERT_NEAR(1, CalculateFragmentCount(12_m, 6_c, 0.5 * CLHEP::MeV, runs), 0.01);
  ASSERT_NEAR(1, CalculateFragmentCount(12_m, 6_c, 0.7 * CLHEP::MeV, runs), 0.01);

  /// Carbons should break into 3 parts [1, 1.4]
  ASSERT_NEAR(3, CalculateFragmentCount(12_m, 6_c, 1 * CLHEP::MeV, runs), 0.01);
  ASSERT_NEAR(3, CalculateFragmentCount(12_m, 6_c, 1.25 * CLHEP::MeV, runs), 0.01);
  ASSERT_NEAR(3, CalculateFragmentCount(12_m, 6_c, 1.4 * CLHEP::MeV, runs), 0.01);


  /// Carbons should break into less than 3 parts [1.5, 3.5]
  ASSERT_LE(CalculateFragmentCount(12_m, 6_c, 1.5 * CLHEP::MeV, runs), 3);
  ASSERT_LE(CalculateFragmentCount(12_m, 6_c, 2 * CLHEP::MeV, runs), 3);
  ASSERT_LE(CalculateFragmentCount(12_m, 6_c, 3 * CLHEP::MeV, runs), 3);
  ASSERT_LE(CalculateFragmentCount(12_m, 6_c, 3.5 * CLHEP::MeV, runs), 3);

  /// Carbons should break into more than 3 parts [5, ...]
  ASSERT_GE(CalculateFragmentCount(12_m, 6_c, 5 * CLHEP::MeV, runs), 3);
  ASSERT_GE(CalculateFragmentCount(12_m, 6_c, 7 * CLHEP::MeV, runs), 3);
  ASSERT_GE(CalculateFragmentCount(12_m, 6_c, 9 * CLHEP::MeV, runs), 3);
  ASSERT_GE(CalculateFragmentCount(12_m, 6_c, 20 * CLHEP::MeV, runs), 3);
}

TEST(FermiBreakUpTests, MomentumConservation) {
  int seed = 5;
  srand(seed);
  int tries = 500;
  size_t runs = 1e2;
  for (int t = 0; t < tries; ++t) {
    MassNumber mass(rand() % 16 + 1);
    ChargeNumber charge(rand() % (int(mass) + 1));
    FermiFloat energy = (rand() % 1000) * CLHEP::MeV * FermiFloat(mass);
    auto vec = Randomizer::IsotropicVector() * (rand() % 1000) * CLHEP::MeV;
    auto mom = LorentzVector(vec, std::sqrt(std::pow(NucleiProperties().GetNuclearMass(mass, charge) + energy, 2) +
        vec.mag2()));
    auto particle = FermiParticle(mass, charge, mom);
    for (size_t i = 0; i < runs; ++i) {
      LorentzVector sum(0, 0, 0, 0);
      auto particles = FermiBreakUp::BreakItUp(particle, true);
      for(auto& fragment : particles) {
        sum += fragment.GetMomentum();
      }
      ASSERT_NEAR(sum.x(), mom.x(), 1e-5);
      ASSERT_NEAR(sum.y(), mom.y(), 1e-5);
      ASSERT_NEAR(sum.z(), mom.z(), 1e-5);
      ASSERT_NEAR(sum.e(), mom.e(), 1e-5);
    }
  }
}

TEST(FermiBreakUpTests, BaryonConservation) {
  int seed = 5;
  srand(seed);
  int tries = 500;
  size_t runs = 1e2;
  for (int t = 0; t < tries; ++t) {
    MassNumber mass(rand() % 16 + 1);
    ChargeNumber charge(rand() % (int(mass) + 1));
    FermiFloat energy = (rand() % 1000) * CLHEP::MeV * FermiFloat(mass);
    auto vec = Randomizer::IsotropicVector() * (rand() % 1000) * CLHEP::MeV;
    auto mom = LorentzVector(vec, std::sqrt(std::pow(NucleiProperties().GetNuclearMass(mass, charge) + energy, 2) +
        vec.mag2()));
    auto particle = FermiParticle(mass, charge, mom);
    for (size_t i = 0; i < runs; ++i) {
      MassNumber mass_sum(0);
      ChargeNumber charge_sum(0);
      auto particles = FermiBreakUp::BreakItUp(particle, true);
      for(auto& fragment : particles) {
        mass_sum = MassNumber(FermiUInt(mass_sum) + FermiUInt(fragment.GetMassNumber()));
        charge_sum = ChargeNumber(FermiUInt(charge_sum) + FermiUInt(fragment.GetChargeNumber()));
      }
      ASSERT_EQ(mass_sum, mass);
      ASSERT_EQ(charge_sum, charge);
    }
  }
}