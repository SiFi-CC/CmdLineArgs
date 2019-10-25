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
  \file   CmdLineOption.cc
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

#include "CmdLineConfig.hh"
#include "CmdLineOption.hh"

const TString CmdLineOption::delim = ": ,";

Bool_t CmdLineOption::AbortOnWarning = kFALSE;

CmdLineOption::CmdLineOption(const char* name, const char* cmd,
                             const char* help, void (*f)()) {
  Init(name, cmd, help);
  fDefInt = kFALSE;
  fType = kFlag;
  fFunction = f;
}

CmdLineOption::CmdLineOption(const char* name, const char* cmd,
                             const char* help, Bool_t defval, void (*f)()) {
  Init(name, cmd, help);
  fDefInt = (defval == kTRUE ? 1 : 0);
  fType = kBool;
  fFunction = f;
}

CmdLineOption::CmdLineOption(const char* name, const char* cmd,
                             const char* help, Int_t defval, void (*f)()) {
  Init(name, cmd, help);
  fDefInt = defval;
  fType = kInt;
  fFunction = f;
}

CmdLineOption::CmdLineOption(const char* name, const char* cmd,
                             const char* help, Double_t defval, void (*f)()) {
  Init(name, cmd, help);
  fDefDouble = defval;
  fType = kDouble;
  fFunction = f;
}

CmdLineOption::CmdLineOption(const char* name, const char* cmd,
                             const char* help, const char* defval,
                             void (*f)()) {
  Init(name, cmd, help);
  fDefString = defval;
  fType = kStringNotChecked;
  fFunction = f;
}

CmdLineOption::CmdLineOption(const char* name)
    : CmdLineOption(name, 0, 0, kFALSE, nullptr) {}

CmdLineOption::CmdLineOption(const char* name, Bool_t defval)
    : CmdLineOption(name, 0, 0, defval, nullptr) {}

CmdLineOption::CmdLineOption(const char* name, Int_t defval)
    : CmdLineOption(name, 0, 0, defval, nullptr) {}

CmdLineOption::CmdLineOption(const char* name, Double_t defval)
    : CmdLineOption(name, 0, 0, defval, nullptr) {}

CmdLineOption::CmdLineOption(const char* name, const char* defval)
    : CmdLineOption(name, 0, 0, defval, nullptr) {}

CmdLineOption::CmdLineOption(const CmdLineOption& ref) {
  MayNotUse("CmdLineOption(const CmdLineOption& ref)");
}

CmdLineOption::CmdLineOption() { Init(0, 0, 0); };

CmdLineOption::~CmdLineOption() {
  TObject* obj =
      CmdLineConfig::instance()->GetEnv()->Lookup("CmdLine." + fName);
  if (obj) CmdLineConfig::instance()->GetEnv()->GetTable()->Remove(obj);

  if (!fName.IsNull()) CmdLineConfig::instance()->Remove(this);
}

CmdLineOption* CmdLineOption::Expand(TObject* obj) {
  if (obj == 0) return nullptr;
  TString cname = obj->ClassName();
  TString name = obj->GetName();
  return Expand(cname, name);
}

CmdLineOption* CmdLineOption::Expand(const TString& cname,
                                     const TString& name) {
  TString newname = cname + "." + name + "." + fName;
  CmdLineOption* newopt = CmdLineConfig::instance()->FindOption(newname);
  if (newopt != 0) return newopt;
  switch (fType) {
    case kFlag:
      newopt = new CmdLineOption(newname, kFALSE);
      break;
    case kBool:
      newopt = new CmdLineOption(newname, (Bool_t)fDefInt);
      break;
    case kInt:
      newopt = new CmdLineOption(newname, fDefInt);
      break;
    case kDouble:
      newopt = new CmdLineOption(newname, fDefDouble);
      break;
    case kString:
    case kStringNotChecked:
      newopt = new CmdLineOption(newname, fDefString);
      break;
    default:
      newopt = 0;
  }
  return newopt;
}

void CmdLineOption::Init(const char* name, const char* cmd, const char* help) {
  if (0 == strlen(name)) return;

  fName = name;
  fCmdArg = cmd;
  fHelp = help;
  fDefInt = 0;
  fDefDouble = 0.;
  fDefString = "";
  fType = kNone;
  fFunction = 0;

  CmdLineConfig::instance()->Insert(this);
}

const Int_t CmdLineOption::GetArraySizeFromString(const TString arraystring) {
  TObjArray* Items = arraystring.Tokenize(delim);
  Int_t NrValues = Items->GetEntries();
  delete Items;
  return NrValues;
}

const Int_t CmdLineOption::GetIntArrayValueFromString(const TString arraystring,
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
CmdLineOption::GetDoubleArrayValueFromString(const TString arraystring,
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

const char* CmdLineOption::GetHelp() const { return fHelp.Data(); };

const Bool_t CmdLineOption::GetFlagValue() const {
  if (fType != kFlag)
    std::cerr << "CmdLineOption: " << fName << " not defined as flag! "
              << std::endl;
  if (GetValue("CmdLine." + fName, kFALSE) == 1) return kTRUE;
  return kFALSE;
}

const Bool_t CmdLineOption::GetBoolValue() const {
  if (fType != kBool)
    std::cerr << "CmdLineOption: " << fName << " not defined as bool! "
              << std::endl;
  if (GetValue("CmdLine." + fName, fDefInt) == 1) return kTRUE;
  return kFALSE;
}

const Int_t CmdLineOption::GetIntValue() const {
  if (fType != kInt)
    std::cerr << "CmdLineOption: " << fName << " not defined as integer!"
              << std::endl;
  return GetValue("CmdLine." + fName, fDefInt);
}

const Int_t CmdLineOption::GetIntArrayValue(const Int_t index) {
  const TString arraystring = GetStringValue(kTRUE);
  return GetIntArrayValueFromString(arraystring, index);
}

const Double_t CmdLineOption::GetDoubleValue() const {
  if (fType != kDouble)
    std::cerr << "CmdLineOption: " << fName << " not defined as double!"
              << std::endl;
  return GetValue("CmdLine." + fName, fDefDouble);
}

const Double_t CmdLineOption::GetDoubleArrayValue(const Int_t index) {
  const TString arraystring = GetStringValue(kTRUE);
  return GetDoubleArrayValueFromString(arraystring, index);
}

const Int_t CmdLineOption::GetArraySize() {
  const TString arraystring = GetStringValue(kTRUE);
  return GetArraySizeFromString(arraystring);
}

const char* CmdLineOption::GetStringValue(Bool_t arrayParsing) {
  if (fType == kStringNotChecked) {
    const char* envVal = CmdLineConfig::instance()->GetEnv()->GetValue(
        "CmdLine." + fName, (const char*)0);
    if (envVal != 0) {
      TString tmpString = envVal;
      TString corString = tmpString.Strip();
      CmdLineConfig::instance()->GetEnv()->SetValue("CmdLine." + fName,
                                                    corString.Data());
    }
    fType = kString;
  }
  if (!arrayParsing and fType != kString) {
    std::cerr << "CmdLineOption: " << fName << " not defined as char*!"
              << std::endl;
    if (AbortOnWarning) abort();
  }
  if (fDefString.IsNull())
    return GetValue("CmdLine." + fName, (const char*)0);
  else
    return GetValue("CmdLine." + fName, fDefString.Data());
}

const Bool_t CmdLineOption::GetDefaultBoolValue() const {
  if (fType != kBool)
    std::cerr << "CmdLineOption: " << fName << " not defined as bool! "
              << std::endl;
  return (Bool_t)fDefInt;
}

const Int_t CmdLineOption::GetDefaultIntValue() const {
  if (fType != kInt)
    std::cerr << "CmdLineOption: " << fName << " not defined as integer!"
              << std::endl;
  return fDefInt;
}

const Int_t CmdLineOption::GetDefaultIntArrayValue(const Int_t index) const {
  const TString arraystring = GetDefaultStringValue(kTRUE);
  return GetIntArrayValueFromString(arraystring, index);
}

const Double_t CmdLineOption::GetDefaultDoubleValue() const {
  if (fType != kDouble)
    std::cerr << "CmdLineOption: " << fName << " not defined as double!"
              << std::endl;
  return fDefDouble;
}

const Double_t
CmdLineOption::GetDefaultDoubleArrayValue(const Int_t index) const {
  const TString arraystring = GetDefaultStringValue(kTRUE);
  return GetDoubleArrayValueFromString(arraystring, index);
}

const Int_t CmdLineOption::GetDefaultArraySize() const {
  const TString arraystring = GetDefaultStringValue(kTRUE);
  return GetArraySizeFromString(arraystring);
}

const char* CmdLineOption::GetDefaultStringValue(Bool_t arrayparsing) const {
  if (!arrayparsing and fType != kString && fType != kStringNotChecked)
    std::cerr << "CmdLineOption: " << fName << " not defined as char*!"
              << std::endl;
  if (fDefString.IsNull())
    return (const char*)nullptr;
  else
    return fDefString.Data();
}

const Bool_t CmdLineOption::GetFlagValue(const char* name) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetFlagValue();
  return kFALSE;
}

const Bool_t CmdLineOption::GetBoolValue(const char* name) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetBoolValue();
  return kFALSE;
}

const Int_t CmdLineOption::GetIntValue(const char* name) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetIntValue();
  return 0;
}

const Int_t CmdLineOption::GetIntArrayValue(const char* name,
                                            const Int_t index) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetIntArrayValue(index);
  return 0;
}

const Double_t CmdLineOption::GetDoubleValue(const char* name) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetDoubleValue();
  return 0.;
}

const Double_t CmdLineOption::GetDoubleArrayValue(const char* name,
                                                  const Int_t index) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetDoubleArrayValue(index);
  return 0;
}

const Int_t CmdLineOption::GetArraySize(const char* name) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetArraySize();
  return 0;
}

const char* CmdLineOption::GetStringValue(const char* name) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetStringValue();
  return nullptr;
}

const Bool_t CmdLineOption::GetDefaultBoolValue(const char* name) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry->fName == name) return entry->GetDefaultBoolValue();
  return kFALSE;
}

const Int_t CmdLineOption::GetDefaultIntValue(const char* name) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetDefaultIntValue();
  return 0;
}

const Int_t CmdLineOption::GetDefaultIntArrayValue(const char* name,
                                                   const Int_t index) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetDefaultIntArrayValue(index);
  return 0;
}

const Double_t CmdLineOption::GetDefaultDoubleValue(const char* name) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetDefaultDoubleValue();
  return 0.;
}

const Double_t CmdLineOption::GetDefaultDoubleArrayValue(const char* name,
                                                         const Int_t index) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetDefaultDoubleArrayValue(index);
  return 0;
}

const Int_t CmdLineOption::GetDefaultArraySize(const char* name) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetDefaultArraySize();
  return 0;
}

const char* CmdLineOption::GetDefaultStringValue(const char* name) {
  CmdLineOption* entry = CmdLineConfig::instance()->FindOption(name);
  if (entry) return entry->GetDefaultStringValue();
  return nullptr;
}

void CmdLineOption::PrintHelp() {
  if (fCmdArg == "") return;
  std::cout << "  " << resetiosflags(std::ios::adjustfield)
            << setiosflags(std::ios::left) << std::setw(20) << fCmdArg << fHelp
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

void CmdLineOption::Print() {
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

const char* CmdLineOption::Getvalue(const char* name) const {
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
Int_t CmdLineOption::GetValue(const char* name, Int_t dflt) const {
  // Returns the integer value for a resource. If the resource is not found
  // return the dflt value.

  const char* cp = CmdLineOption::Getvalue(name);
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
Double_t CmdLineOption::GetValue(const char* name, Double_t dflt) const {
  // Returns the double value for a resource. If the resource is not found
  // return the dflt value.

  const char* cp = CmdLineOption::Getvalue(name);
  if (cp) {
    char* endptr;
    Double_t val = strtod(cp, &endptr);
    if ((0.0 == val) && (cp == endptr)) return dflt;
    return val;
  }
  return dflt;
}

//______________________________________________________________________________
const char* CmdLineOption::GetValue(const char* name, const char* dflt) const {
  // Returns the character value for a named resouce. If the resource is
  // not found the dflt value is returned.

  const char* cp = CmdLineOption::Getvalue(name);
  if (cp) return cp;
  return dflt;
}
