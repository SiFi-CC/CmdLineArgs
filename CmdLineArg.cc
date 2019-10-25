/******************************************************************************
 *
 * $Id$
 *
 * Environment:
 *    Software development for ANKE detector system at COSY
 *
 * Author List:
 *    Volker Hejny                Original author
 *    Rafał Lalik                 Modifications, creation of CmdLineArgs library
 *
 * Copyright Information:
 *    Copyright (C) 2002          Institut für Kernphysik
 *                                Forschungszentrum Jülich
 *    Copyright (C) 2018          Rafał Lalik, Jagiellonian University Kraków
 *
 *****************************************************************************/

/*!
  \file   CmdLineArg.cc
  \brief

  <long description>

  \author Volker Hejny
  \date   2002-07-29
  \author Rafał Lalik
  \date   2019-03-01
*/

#include <cstdlib>
#include <iomanip>
#include <iostream>

#include <TEnv.h>
#include <THashList.h>
#include <TList.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TSystem.h>

#include "CmdLineArg.hh"
#include "CmdLineConfig.hh"

const TString CmdLineArg::delim = ": ,";

CmdLineArg::CmdLineArg(const char* name, const char* help, OptionType type,
                       void (*f)(), bool greedy) {
  Init(name, help, greedy);
  fType = type;
  fFunction = f;
}

CmdLineArg::CmdLineArg(const char* name, OptionType type)
    : CmdLineArg(name, 0, type, nullptr) {}

CmdLineArg::CmdLineArg(const CmdLineArg& ref) {
  MayNotUse("CmdLineArg(const CmdLineArg& ref)");
}

CmdLineArg::CmdLineArg() { Init(0, 0); };

CmdLineArg::~CmdLineArg() {
  TObject* obj =
      CmdLineConfig::instance()->GetEnv()->Lookup("CmdLine." + fName);
  if (obj) CmdLineConfig::instance()->GetEnv()->GetTable()->Remove(obj);

  if (!fName.IsNull()) CmdLineConfig::instance()->Remove(this);
}

CmdLineArg* CmdLineArg::Expand(TObject* obj) {
  if (obj == 0) return nullptr;
  TString cname = obj->ClassName();
  TString name = obj->GetName();
  return Expand(cname, name);
}

CmdLineArg* CmdLineArg::Expand(const TString& cname, const TString& name) {
  TString newname = cname + "." + name + "." + fName;
  CmdLineArg* newopt = CmdLineConfig::instance()->FindArgument(newname);
  if (newopt != 0) return newopt;
  return new CmdLineArg(newname, fType);
}

void CmdLineArg::Init(const char* name, const char* help, bool greedy) {
  fName = name;
  fHelp = help;
  fType = kNone;
  fFunction = 0;

  if (!greedy) CmdLineConfig::instance()->Insert(this);
}

const Int_t CmdLineArg::GetArraySizeFromString(const TString arraystring) {
  TObjArray* Items = arraystring.Tokenize(delim);
  Int_t NrValues = Items->GetEntries();
  delete Items;
  return NrValues;
}

const Int_t CmdLineArg::GetIntArrayValueFromString(const TString arraystring,
                                                   const Int_t index) {
  Int_t returnvalue = 0;

  TObjArray* Items = arraystring.Tokenize(delim);
  Int_t NrValues = Items->GetEntries();
  if (index >= 1 && index <= NrValues)
    returnvalue =
        dynamic_cast<TObjString*>(Items->At(index - 1))->String().Atoi();

  if (Items) delete Items;
  return returnvalue;
}

const Double_t
CmdLineArg::GetDoubleArrayValueFromString(const TString arraystring,
                                          const Int_t index) {
  Double_t returnvalue = 0.;
  TObjArray* Items = arraystring.Tokenize(delim);
  Int_t NrValues = Items->GetEntries();
  if (index >= 1 && index <= NrValues)
    returnvalue =
        dynamic_cast<TObjString*>(Items->At(index - 1))->String().Atof();

  if (Items) delete Items;
  return returnvalue;
}

const char* CmdLineArg::GetHelp() const { return fHelp.Data(); };

const Bool_t CmdLineArg::GetFlagValue() const {
  if (fType != kFlag)
    std::cerr << "CmdLineArg: " << fName << " not defined as flag! "
              << std::endl;
  if (GetValue("CmdLine." + fName, kFALSE) == 1) return kTRUE;
  return fValue.Atoi();
}

const Bool_t CmdLineArg::GetBoolValue() const {
  if (fType != kBool)
    std::cerr << "CmdLineArg: " << fName << " not defined as bool! "
              << std::endl;
  return fValue.Atoi();
}

const Int_t CmdLineArg::GetIntValue() const {
  if (fType != kInt)
    std::cerr << "CmdLineArg: " << fName << " not defined as integer!"
              << std::endl;
  return fValue.Atoi();
}

const Int_t CmdLineArg::GetIntArrayValue(const Int_t index) {
  const TString arraystring = GetStringValue(kTRUE);
  return GetIntArrayValueFromString(arraystring, index);
}

const Double_t CmdLineArg::GetDoubleValue() const {
  if (fType != kDouble)
    std::cerr << "CmdLineArg: " << fName << " not defined as double!"
              << std::endl;
  return fValue.Atof();
}

const Double_t CmdLineArg::GetDoubleArrayValue(const Int_t index) {
  const TString arraystring = GetStringValue(kTRUE);
  return GetDoubleArrayValueFromString(arraystring, index);
}

const Int_t CmdLineArg::GetArraySize() {
  const TString arraystring = GetStringValue(kTRUE);
  return GetArraySizeFromString(arraystring);
}

const char* CmdLineArg::GetStringValue(Bool_t arrayParsing) {
  if (fType == kStringNotChecked) {
    const char* envVal = CmdLineConfig::instance()->GetEnv()->GetValue(
        "CmdLine." + fName, (const char*)0);
    if (envVal != 0) {
      TString tmpString = envVal;
      fValue = tmpString.Strip();
    }
    fType = kString;
  }
  if (!arrayParsing and fType != kString)
    std::cerr << "CmdLineArg: " << fName << " not defined as char*!"
              << std::endl;
  return fValue.Data();
}

const Bool_t CmdLineArg::GetFlagValue(const char* name) {
  CmdLineArg* entry = CmdLineConfig::instance()->FindArgument(name);
  if (entry) return entry->GetFlagValue();
  return kFALSE;
}

const Bool_t CmdLineArg::GetBoolValue(const char* name) {
  CmdLineArg* entry = CmdLineConfig::instance()->FindArgument(name);
  if (entry) return entry->GetBoolValue();
  return kFALSE;
}

const Int_t CmdLineArg::GetIntValue(const char* name) {
  CmdLineArg* entry = CmdLineConfig::instance()->FindArgument(name);
  if (entry) return entry->GetIntValue();
  return 0;
}

const Int_t CmdLineArg::GetIntArrayValue(const char* name, const Int_t index) {
  CmdLineArg* entry = CmdLineConfig::instance()->FindArgument(name);
  if (entry) return entry->GetIntArrayValue(index);
  return 0;
}

const Double_t CmdLineArg::GetDoubleValue(const char* name) {
  CmdLineArg* entry = CmdLineConfig::instance()->FindArgument(name);
  if (entry) return entry->GetDoubleValue();
  return 0.;
}

const Double_t CmdLineArg::GetDoubleArrayValue(const char* name,
                                               const Int_t index) {
  CmdLineArg* entry = CmdLineConfig::instance()->FindArgument(name);
  if (entry) return entry->GetDoubleArrayValue(index);
  return 0;
}

const Int_t CmdLineArg::GetArraySize(const char* name) {
  CmdLineArg* entry = CmdLineConfig::instance()->FindArgument(name);
  if (entry) return entry->GetArraySize();
  return 0;
}

const char* CmdLineArg::GetStringValue(const char* name) {
  CmdLineArg* entry = CmdLineConfig::instance()->FindArgument(name);
  if (entry) return entry->GetStringValue();
  return nullptr;
}

void CmdLineArg::PrintHelp(const char* placeholder) {
  std::cout << "  " << resetiosflags(std::ios::adjustfield)
            << setiosflags(std::ios::left) << " " << std::setw(19)
            << (placeholder ? placeholder : fName.Data()) << fHelp
            << resetiosflags(std::ios::adjustfield);
  switch (fType) {
    case kFlag:
      std::cout << " (flag)";
      break;
    case kBool:
      std::cout << " (bool)";
      break;
    case kInt:
      std::cout << " (int)";
      break;
    case kDouble:
      std::cout << " (double)";
      break;
    case kString:
    case kStringNotChecked:
      std::cout << " (char*)";
      break;
    default:
      std::cout << " (unknown type)";
      break;
  }
  std::cout << std::endl;
}

void CmdLineArg::Print() {
  std::cout << "  " << resetiosflags(std::ios::adjustfield)
            << setiosflags(std::ios::left) << std::setw(20) << fName;
  switch (fType) {
    case kFlag:
      if (GetFlagValue())
        std::cout << "YES";
      else
        std::cout << "NO";
      std::cout << " (bool)";
      break;
    case kBool:
      if (GetBoolValue())
        std::cout << "kTRUE";
      else
        std::cout << "kFALSE";
      std::cout << " (bool)";
      break;
    case kInt:
      std::cout << GetIntValue() << " (int)";
      break;
    case kDouble:
      std::cout << GetDoubleValue() << " (double)";
      break;
    case kString:
    case kStringNotChecked: {
      const char* value = GetStringValue();
      if (value == 0) value = "(null)";
      std::cout << "'" << value << "'"
                << " (char*)";
    } break;
    default:
      std::cout << " ?? (unknown type)";
      break;
  }
  std::cout << resetiosflags(std::ios::adjustfield) << std::endl;
}

const char* CmdLineArg::Getvalue(const char* name) const {
  TEnvRec* tmp = CmdLineConfig::instance()->GetEnv()->Lookup(name);
  if (tmp != 0) return tmp->GetValue();

  // ok, let's try to match wildcards: everything may be replaced by '*'
  // except the first and last component

  TString searchname = name;
  TObjArray* components = searchname.Tokenize(".");
  Int_t n = components->GetEntries();
  Int_t max_count = 1 << (n - 1);
  Int_t count = 2;
  while (count < max_count) {
    TString to_be_checked = "";
    for (Int_t i = 0; i < n; i++) {
      if (to_be_checked > "") to_be_checked += ".";
      if (count & (1 << (n - i - 1))) {
        to_be_checked += "*";
      } else {
        to_be_checked +=
            dynamic_cast<TObjString*>(components->At(i))->GetString();
      }
    }
    // std::cout << "Searching " << to_be_checked << std::endl;
    tmp = CmdLineConfig::instance()->GetEnv()->Lookup(to_be_checked);
    if (tmp) {
      // std::cout << "Found." << std::endl;
      delete components;
      return tmp->GetValue();
    }
    count += 2;
  }
  delete components;
  return nullptr;
}

// code taken from TEnv functions

static struct BoolNameTable_t {
  const char* fName;
  Int_t fValue;
} gBoolNames[] = {{"TRUE", 1}, {"FALSE", 0}, {"ON", 1},  {"OFF", 0}, {"YES", 1},
                  {"NO", 0},   {"OK", 1},    {"NOT", 0}, {0, 0}};

//______________________________________________________________________________
Int_t CmdLineArg::GetValue(const char* name, Int_t dflt) const {
  // Returns the integer value for a resource. If the resource is not found
  // return the dflt value.

  const char* cp = CmdLineArg::Getvalue(name);
  if (cp) {
    char buf2[512], *cp2 = buf2;

    while (isspace((int)*cp))
      cp++;
    if (*cp) {
      BoolNameTable_t* bt;
      if (isdigit((int)*cp) || *cp == '-' || *cp == '+') return atoi(cp);
      while (isalpha((int)*cp))
        *cp2++ = toupper((int)*cp++);
      *cp2 = 0;
      for (bt = gBoolNames; bt->fName; bt++)
        if (strcmp(buf2, bt->fName) == 0) return bt->fValue;
    }
  }
  return dflt;
}

//______________________________________________________________________________
Double_t CmdLineArg::GetValue(const char* name, Double_t dflt) const {
  // Returns the double value for a resource. If the resource is not found
  // return the dflt value.

  const char* cp = CmdLineArg::Getvalue(name);
  if (cp) {
    char* endptr;
    Double_t val = strtod(cp, &endptr);
    if ((0.0 == val) && (cp == endptr)) return dflt;
    return val;
  }
  return dflt;
}

//______________________________________________________________________________
const char* CmdLineArg::GetValue(const char* name, const char* dflt) const {
  // Returns the character value for a named resouce. If the resource is
  // not found the dflt value is returned.

  const char* cp = CmdLineArg::Getvalue(name);
  if (cp) return cp;
  return dflt;
}
