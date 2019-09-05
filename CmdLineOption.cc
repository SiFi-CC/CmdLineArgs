/******************************************************************************
 *
 * $Id$
 *
 * Environment:
 *    Software development for ANKE detector system at COSY
 *
 * Author List:
 *    Volker Hejny                Original author
 *    Rafa? Lalik                 Modifications, creation of CmdLineArgs library
 *
 * Copyright Information:
 *    Copyright (C) 2002          Institut für Kernphysik
 *                                Forschungszentrum Jülich
 *    Copyright (C) 2018          Rafa? Lalik
 *
 *****************************************************************************/

/*!
  \file   CmdLineOption.cc
  \brief

  <long description>

  \author Volker Hejny
  \date   2002-07-29
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

#include "CmdLineOption.hh"

TList* CmdLineOption::fgList = 0;
TEnv* CmdLineOption::fgEnv = 0;
std::vector<TString> CmdLineOption::fgPositional;
const TString CmdLineOption::delim = ": ,";

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
  TObject* obj = GetEnv()->Lookup("CmdLine." + fName);
  if (obj) GetEnv()->GetTable()->Remove(obj);

  if (!fName.IsNull()) fgList->Remove(this);
}

void CmdLineOption::ClearOptions() {
  for (int i = fgList->GetEntries(); i > 2; --i) {
    TObject* obj = fgList->Last();
    fgList->RemoveLast();
    delete obj;
    obj = nullptr;
  }

  fgPositional.clear();
}

CmdLineOption* CmdLineOption::Find(const char* name) {
  if (fgList == 0) return nullptr;
  TIter it(fgList);
  CmdLineOption* entry;
  while ((entry = dynamic_cast<CmdLineOption*>(it()))) {
    if (entry->fName == name) return entry;
  }
  return nullptr;
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
  CmdLineOption* newopt = Find(newname);
  if (newopt != 0) return newopt;
  switch (fType) {
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
  fName = name;
  fCmdArg = cmd;
  fHelp = help;
  fDefInt = 0;
  fDefDouble = 0.;
  fDefString = "";
  fType = kNone;
  fFunction = 0;

  if (fName.IsNull()) return;

  if (fgList == 0) {
    fgList = new TList();
    fgList->SetOwner();
  }

  TIter it(fgList);
  CmdLineOption* entry;
  while ((entry = dynamic_cast<CmdLineOption*>(it()))) {
    if (entry->fName == fName) {
      std::cerr << "CmdLineOption: option '" << fName
                << "' already exists -> fix it" << std::endl;
      exit(1);
    }
    if (fCmdArg != "" && entry->fCmdArg == fCmdArg) {
      std::cerr << "CmdLineOption: options '" << fName << "' and '"
                << entry->fName << "' share tag '" << fCmdArg << "'"
                << std::endl;
    }
  }

  fgList->Add(this);
}

void CmdLineOption::CheckCmdLine(int argc, char** argv) {
  if (argc < 2) return;

  if (fgList == 0) return;

  CmdLineOption* entry = nullptr;

  for (Int_t i = 1; i < argc; i++) {
    Bool_t isCmdLine = kFALSE;
    if (CheckCmdLineSpecial(argc, argv, i)) continue;

    TIter it(fgList);
    while ((entry = dynamic_cast<CmdLineOption*>(it()))) {
      if (entry->fCmdArg == "") continue;

      if (entry->fCmdArg == argv[i]) {
        isCmdLine = kTRUE;
        if (i < argc - 1)
          GetEnv()->SetValue("CmdLine." + entry->fName, argv[++i]);
        if (entry->fFunction != 0) (*entry->fFunction)();
        break;
      }
    }

    if (!isCmdLine) fgPositional.push_back(argv[i]);
  }
}

Bool_t CmdLineOption::CheckCmdLineSpecial(int argc, char** argv, int i) {
  TString option = argv[i];
  if (option == "-h") {
    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
    std::cout << "  -h                  show this help" << std::endl;
    PrintHelp();
    exit(0);
    return kTRUE;
  } else if (option == "-p") {
    Print();
    return kTRUE;
  } else if (option == "-extra-sorterrc") {
    if ((i + 1) < argc) {
      TString includepath = "";
      if (GetStringValue("IncludePath")) {
        includepath = gSystem->ExpandPathName(GetStringValue("IncludePath"));
        if (!(includepath.EndsWith("/"))) includepath += "/";
      }
      TString extrafile = argv[i + 1];
      TString extra = gSystem->ExpandPathName(extrafile.Data());
      if (!(extra.BeginsWith("/") || extra.BeginsWith("./"))) {
        extra = includepath + extra;
      }
      if (gSystem->AccessPathName(extra)) {
        std::cout << "Error: extra rc path not accessible (" << extra << ")"
                  << std::endl;
        exit(0);
      } else {
        void* dirp = gSystem->OpenDirectory(extra);
        if (dirp == 0) {
          std::cout << "Reading extra rc file: " << extra << std::endl;
          fgEnv->ReadFile(extra, kEnvChange);
        } else {
          if (!extra.EndsWith("/")) extra += "/";
          const char* localname = 0;
          while ((localname = gSystem->GetDirEntry(dirp))) {
            TString strName = localname;
            if (strName.EndsWith(".rc")) {
              std::cout << "Reading extra fc file: " << extra + strName
                        << std::endl;
              fgEnv->ReadFile(extra + strName, kEnvChange);
            }
          }
          gSystem->FreeDirectory(dirp);
        }
      }
    }
    return kTRUE;
  } // end extra sorterrc
  return kFALSE;
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

  const TString arraystring = GetStringValue();
  return GetIntArrayValueFromString(arraystring, index);
}

const Double_t CmdLineOption::GetDoubleValue() const {
  if (fType != kDouble)
    std::cerr << "CmdLineOption: " << fName << " not defined as double!"
              << std::endl;
  return GetValue("CmdLine." + fName, fDefDouble);
}

const Double_t CmdLineOption::GetDoubleArrayValue(const Int_t index) {

  const TString arraystring = GetStringValue();
  return GetDoubleArrayValueFromString(arraystring, index);
}

const Int_t CmdLineOption::GetArraySize() {
  const TString arraystring = GetStringValue();
  return GetArraySizeFromString(arraystring);
}

const char* CmdLineOption::GetStringValue() {
  if (fType == kStringNotChecked) {
    const char* envVal = GetEnv()->GetValue("CmdLine." + fName, (const char*)0);
    if (envVal != 0) {
      TString tmpString = envVal;
      TString corString = tmpString.Strip();
      GetEnv()->SetValue("CmdLine." + fName, corString.Data());
    }
    fType = kString;
  }
  if (fType != kString)
    std::cerr << "CmdLineOption: " << fName << " not defined as char*!"
              << std::endl;
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

const Int_t CmdLineOption::GetDefaultIntArrayValue(const Int_t index) {
  const TString arraystring = GetDefaultStringValue();
  return GetIntArrayValueFromString(arraystring, index);
}

const Double_t CmdLineOption::GetDefaultDoubleValue() const {
  if (fType != kDouble)
    std::cerr << "CmdLineOption: " << fName << " not defined as double!"
              << std::endl;
  return fDefDouble;
}

const Double_t CmdLineOption::GetDefaultDoubleArrayValue(const Int_t index) {
  const TString arraystring = GetDefaultStringValue();
  return GetDoubleArrayValueFromString(arraystring, index);
}

const Int_t CmdLineOption::GetDefaultArraySize() {
  const TString arraystring = GetDefaultStringValue();
  return GetArraySizeFromString(arraystring);
}

const char* CmdLineOption::GetDefaultStringValue() const {
  if (fType != kString && fType != kStringNotChecked)
    std::cerr << "CmdLineOption: " << fName << " not defined as char*!"
              << std::endl;
  if (fDefString.IsNull())
    return (const char*)nullptr;
  else
    return fDefString.Data();
}

const Bool_t CmdLineOption::GetBoolValue(const char* name) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetBoolValue();
  return kFALSE;
}

const Int_t CmdLineOption::GetIntValue(const char* name) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetIntValue();
  return 0;
}

const Int_t CmdLineOption::GetIntArrayValue(const char* name,
                                            const Int_t index) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetIntArrayValue(index);
  return 0;
}

const Double_t CmdLineOption::GetDoubleValue(const char* name) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetDoubleValue();
  return 0.;
}

const Double_t CmdLineOption::GetDoubleArrayValue(const char* name,
                                                  const Int_t index) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetDoubleArrayValue(index);
  return 0;
}

const Int_t CmdLineOption::GetArraySize(const char* name) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetArraySize();
  return 0;
}

const char* CmdLineOption::GetStringValue(const char* name) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetStringValue();
  return nullptr;
}

const Bool_t CmdLineOption::GetDefaultBoolValue(const char* name) {
  CmdLineOption* entry = Find(name);
  if (entry->fName == name) return entry->GetDefaultBoolValue();
  return kFALSE;
}

const Int_t CmdLineOption::GetDefaultIntValue(const char* name) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetDefaultIntValue();
  return 0;
}

const Int_t CmdLineOption::GetDefaultIntArrayValue(const char* name,
                                                   const Int_t index) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetDefaultIntArrayValue(index);
  return 0;
}

const Double_t CmdLineOption::GetDefaultDoubleValue(const char* name) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetDefaultDoubleValue();
  return 0.;
}

const Double_t CmdLineOption::GetDefaultDoubleArrayValue(const char* name,
                                                         const Int_t index) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetDefaultDoubleArrayValue(index);
  return 0;
}

const Int_t CmdLineOption::GetDefaultArraySize(const char* name) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetDefaultArraySize();
  return 0;
}

const char* CmdLineOption::GetDefaultStringValue(const char* name) {
  CmdLineOption* entry = Find(name);
  if (entry) return entry->GetDefaultStringValue();
  return nullptr;
}

void CmdLineOption::PrintHelp() {
  if (fgList == 0) return;
  TIter it(fgList);
  CmdLineOption* entry;
  while ((entry = dynamic_cast<CmdLineOption*>(it()))) {
    if (entry->fCmdArg == "") continue;
    std::cout << "  " << resetiosflags(std::ios::adjustfield)
              << setiosflags(std::ios::left) << std::setw(20) << entry->fCmdArg
              << entry->fHelp << resetiosflags(std::ios::adjustfield);
    switch (entry->fType) {
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
}

void CmdLineOption::Print() {
  if (fgList == 0) return;
  std::cout << "Current settings:" << std::endl;
  TIter it(fgList);
  CmdLineOption* entry;
  while ((entry = dynamic_cast<CmdLineOption*>(it()))) {
    std::cout << "  " << resetiosflags(std::ios::adjustfield)
              << setiosflags(std::ios::left) << std::setw(20) << entry->fName;
    switch (entry->fType) {
      case kBool:
        if (entry->GetBoolValue())
          std::cout << "kTRUE";
        else
          std::cout << "kFALSE";
        std::cout << " (bool)";
        break;
      case kInt:
        std::cout << entry->GetIntValue();
        std::cout << " (int)";
        break;
      case kDouble:
        std::cout << entry->GetDoubleValue();
        std::cout << " (double)";
        break;
      case kString:
      case kStringNotChecked: {
        const char* value = entry->GetStringValue();
        if (value == 0) value = "(null)";
        std::cout << "'" << value << "'";
        std::cout << " (char*)";
      } break;
      default:
        std::cout << " ?? (unknown type)";
        break;
    }
    std::cout << resetiosflags(std::ios::adjustfield) << std::endl;
  }
}

void CmdLineOption::ReadCmdLine(int argc, char** argv) {
  GetEnv();
  fgPositional.clear();
  CheckCmdLine(argc, argv);
}

TEnv* CmdLineOption::GetEnv() {
  if (fgEnv != 0) return fgEnv;

  fgEnv = new TEnv(".sorterrc");
  TString defaultpath = "";
  if (GetStringValue("DefaultPath")) {
    defaultpath = gSystem->ExpandPathName(GetStringValue("DefaultPath"));
    if (gSystem->AccessPathName(defaultpath)) {
      std::cerr << "Error: default path not accessible (" << defaultpath << ")"
                << std::endl;
    } else {
      void* dirp = gSystem->OpenDirectory(defaultpath);
      if (dirp == 0) {
        std::cerr << "Error: default path not readable (" << defaultpath << ")"
                  << std::endl;
      } else {
        std::cout << "Reading defaults from " << defaultpath << std::endl;
        if (!defaultpath.EndsWith("/")) defaultpath += "/";
        const char* localname = 0;
        while ((localname = gSystem->GetDirEntry(dirp))) {
          TString strName = localname;
          if (strName.EndsWith(".rc")) {
            fgEnv->ReadFile(defaultpath + strName, kEnvGlobal);
          }
        }
        gSystem->FreeDirectory(dirp);
      }
    }
  }
  TString includepath = "";
  if (GetStringValue("IncludePath")) {
    includepath = gSystem->ExpandPathName(GetStringValue("IncludePath"));
    if (!(includepath.EndsWith("/"))) includepath += "/";
  }
  if (GetStringValue("Include")) {
    TString includes = gSystem->ExpandPathName(GetStringValue("Include"));
    TObjArray* includesArray = includes.Tokenize(" ");
    TObjString* objString;
    TIter it(includesArray);
    while ((objString = dynamic_cast<TObjString*>(it.Next()))) {
      TString filename = objString->GetString();
      if (!(filename.BeginsWith("/") || filename.BeginsWith("./"))) {
        filename = includepath + filename;
      }
      if (gSystem->AccessPathName(filename)) {
        std::cerr << "Error: rc-file not readable (" << filename << ")"
                  << std::endl;
      } else {
        void* dirp = gSystem->OpenDirectory(filename);
        if (dirp == 0) {
          std::cout << "Reading " << filename << std::endl;
          fgEnv->ReadFile(filename, kEnvUser);
        } else {
          if (!filename.EndsWith("/")) filename += "/";
          const char* localname = 0;
          while ((localname = gSystem->GetDirEntry(dirp))) {
            TString strName = localname;
            if (strName.EndsWith(".rc")) {
              std::cout << "Reading " << filename + strName << std::endl;
              fgEnv->ReadFile(filename + strName, kEnvUser);
            }
          }
          gSystem->FreeDirectory(dirp);
        }
      }
    }
    delete includesArray;
  }
  // work-around because values in these files are overwritten by
  // values in "Defaults" directory
  char* s = gSystem->ConcatFileName(gSystem->HomeDirectory(), ".sorterrc");
  fgEnv->ReadFile(s, kEnvChange);
  delete[] s;
  fgEnv->ReadFile(".sorterrc", kEnvChange);
  return fgEnv;
}

const char* CmdLineOption::Getvalue(const char* name) const {
  TEnvRec* tmp = GetEnv()->Lookup(name);
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
    tmp = GetEnv()->Lookup(to_be_checked);
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
