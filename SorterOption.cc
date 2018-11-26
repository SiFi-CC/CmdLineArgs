/******************************************************************************
 *
 * $Id$
 *
 * Environment:
 *    Software development for ANKE detector system at COSY
 *
 * Author List:
 *    Volker Hejny                Original author
 *
 * Copyright Information:
 *    Copyright (C) 2002          Institut für Kernphysik
 *                                Forschungszentrum Jülich
 *
 *****************************************************************************/

/*!
  \file   SorterOption.cc
  \brief

  <long description>

  \author Volker Hejny
  \date   2002-07-29
*/

#include <cstdlib>
#include <iostream>

#include <TEnv.h>
#include <TList.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TSystem.h>

// #include "CConstBase.hh"
// #include "CLog.hh"

// #include "RSEnv.hh"
#include "SorterOption.hh"

ClassImp(SorterOption);

TList* SorterOption::fgList = 0;
RSEnv* SorterOption::fgEnv = 0;

SorterOption::SorterOption(const char* name, const char* cmd, const char* help,
                           Bool_t defval, void (*f)()) {
  Init(name, cmd, help);
  fDefInt = (defval == kTRUE ? 1 : 0);
  fType = kBool;
  fFunction = f;
};

SorterOption::SorterOption(const char* name, const char* cmd, const char* help,
                           Int_t defval, void (*f)()) {
  Init(name, cmd, help);
  fDefInt = defval;
  fType = kInt;
  fFunction = f;
};

SorterOption::SorterOption(const char* name, const char* cmd, const char* help,
                           Double_t defval, void (*f)()) {
  Init(name, cmd, help);
  fDefDouble = defval;
  fType = kDouble;
  fFunction = f;
};

SorterOption::SorterOption(const char* name, const char* cmd, const char* help,
                           const char* defval, void (*f)()) {
  Init(name, cmd, help);
  fDefString = defval;
  fType = kStringNotChecked;
  fFunction = f;
};

SorterOption::SorterOption(const char* name, Bool_t defval) {
  Init(name, 0, 0);
  fDefInt = (defval == kTRUE ? 1 : 0);
  fType = kBool;
};

SorterOption::SorterOption(const char* name, Int_t defval) {
  Init(name, 0, 0);
  fDefInt = defval;
  fType = kInt;
};

SorterOption::SorterOption(const char* name, Double_t defval) {
  Init(name, 0, 0);
  fDefDouble = defval;
  fType = kDouble;
};

SorterOption::SorterOption(const char* name, const char* defval) {
  Init(name, 0, 0);
  fDefString = defval;
  fType = kStringNotChecked;
};

SorterOption::SorterOption(const SorterOption& ref) {
  MayNotUse("SorterOption(const SorterOption& ref)");
}

SorterOption::SorterOption() { Init(0, 0, 0); };

SorterOption::~SorterOption() {
  if (!fName.IsNull()) fgList->Remove(this);
};

SorterOption* SorterOption::Find(const char* name) {
  if (fgList == 0) return 0;
  TIter it(fgList);
  SorterOption* entry;
  while ((entry = dynamic_cast<SorterOption*>(it()))) {
    if (entry->fName == name) return entry;
  }
  return 0;
}

SorterOption* SorterOption::Expand(TObject* obj) {
  if (obj == 0) return 0;
  TString cname = obj->ClassName();
  TString name = obj->GetName();
  return Expand(cname, name);
}

SorterOption* SorterOption::Expand(const TString& cname, const TString& name) {
  TString newname = cname + "." + name + "." + fName;
  SorterOption* newopt = Find(newname);
  if (newopt != 0) return newopt;
  switch (fType) {
    case kBool:
      newopt = new SorterOption(newname, (Bool_t)fDefInt);
      break;
    case kInt:
      newopt = new SorterOption(newname, fDefInt);
      break;
    case kDouble:
      newopt = new SorterOption(newname, fDefDouble);
      break;
    case kString:
    case kStringNotChecked:
      newopt = new SorterOption(newname, fDefString);
      break;
    default:
      newopt = 0;
  }
  return newopt;
}

void SorterOption::Init(const char* name, const char* cmd, const char* help) {
  fName = name;
  fCmdArg = cmd;
  fHelp = help;
  fDefInt = 0;
  fDefDouble = 0.;
  fDefString = "";
  fType = kNone;
  fFunction = 0;

  if (fName.IsNull()) return;

  if (fgList == 0) fgList = new TList();

  TIter it(fgList);
  SorterOption* entry;
  while ((entry = dynamic_cast<SorterOption*>(it()))) {
    if (entry->fName == fName) {
      std::cerr << "SorterOption: option '" << fName
                << "' already exists -> fix it" << std::endl;
      exit(1);
    }
    if (fCmdArg != "" && entry->fCmdArg == fCmdArg) {
      std::cerr << "SorterOption: options '" << fName << "' and '"
                << entry->fName << "' share tag '" << fCmdArg << "'"
                << std::endl;
    }
  }

  fgList->Add(this);
};

void SorterOption::CheckCmdLine(int argc, char** argv) {
  if (argc < 2) return;
  for (Int_t i = 1; i < argc; i++) {
    if (fCmdArg == argv[i]) {
      if (fType == kBool)
        GetEnv()->SetValue("Sorter." + fName, 1);
      else if (i < argc - 1)
        GetEnv()->SetValue("Sorter." + fName, argv[i + 1]);
      if (fFunction != 0) (*fFunction)();
    }
  }
};

const Int_t SorterOption::GetArraySizeFromString(const TString arraystring) {
  TString delim = ": ";
  TObjArray* Items = arraystring.Tokenize(delim);
  Int_t NrValues = Items->GetEntries();
  delete Items;
  return NrValues;
}

const Int_t SorterOption::GetIntArrayValueFromString(const TString arraystring,
                                                     const Int_t index) {
  TString delim = ": ";
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
SorterOption::GetDoubleArrayValueFromString(const TString arraystring,
                                            const Int_t index) {
  TString delim = ": ";
  Double_t returnvalue = 0.;
  TObjArray* Items = arraystring.Tokenize(delim);
  Int_t NrValues = Items->GetEntries();
  if (index >= 1 && index <= NrValues)
    returnvalue =
        dynamic_cast<TObjString*>(Items->At(index - 1))->String().Atof();

  if (Items) delete Items;
  return returnvalue;
}

const char* SorterOption::GetHelp() const { return fHelp.Data(); };

const Bool_t SorterOption::GetBoolValue() const {
  if (fType != kBool)
    gLog(CLog::kWarning) << "SorterOption: " << fName
                         << " not defined as bool! " << fType << CLog::endl;
  if (GetValue("Sorter." + fName, fDefInt) == 1) return kTRUE;
  return kFALSE;
};

const Int_t SorterOption::GetIntValue() const {
  if (fType != kInt)
    gLog(CLog::kWarning) << "SorterOption: " << fName
                         << " not defined as integer!" << CLog::endl;
  return GetValue("Sorter." + fName, fDefInt);
};

const Int_t SorterOption::GetIntArrayValue(const Int_t index) {

  const TString arraystring = GetStringValue();
  return GetIntArrayValueFromString(arraystring, index);
}

const Double_t SorterOption::GetDoubleValue() const {
  if (fType != kDouble)
    gLog(CLog::kWarning) << "SorterOption: " << fName
                         << " not defined as double!" << CLog::endl;
  return GetValue("Sorter." + fName, fDefDouble);
};

const Double_t SorterOption::GetDoubleArrayValue(const Int_t index) {

  const TString arraystring = GetStringValue();
  return GetDoubleArrayValueFromString(arraystring, index);
}

const Int_t SorterOption::GetArraySize() {
  const TString arraystring = GetStringValue();
  return GetArraySizeFromString(arraystring);
}

const char* SorterOption::GetStringValue() {
  if (fType == kStringNotChecked) {
    const char* envVal = GetEnv()->GetValue("Sorter." + fName, (const char*)0);
    if (envVal != 0) {
      TString tmpString = envVal;
      TString corString = tmpString.Strip();
      GetEnv()->SetValue("Sorter." + fName, corString.Data());
    }
    fType = kString;
  }
  if (fType != kString)
    gLog(CLog::kWarning) << "SorterOption: " << fName
                         << " not defined as char*!" << CLog::endl;
  if (fDefString.IsNull())
    return GetValue("Sorter." + fName, (const char*)0);
  else
    return GetValue("Sorter." + fName, fDefString.Data());
};

const Bool_t SorterOption::GetDefaultBoolValue() const {
  if (fType != kBool)
    gLog(CLog::kWarning) << "SorterOption: " << fName
                         << " not defined as bool! " << fType << CLog::endl;
  return (Bool_t)fDefInt;
};

const Int_t SorterOption::GetDefaultIntValue() const {
  if (fType != kInt)
    gLog(CLog::kWarning) << "SorterOption: " << fName
                         << " not defined as integer!" << CLog::endl;
  return fDefInt;
};

const Int_t SorterOption::GetDefaultIntArrayValue(const Int_t index) {
  const TString arraystring = GetDefaultStringValue();
  return GetIntArrayValueFromString(arraystring, index);
}

const Double_t SorterOption::GetDefaultDoubleValue() const {
  if (fType != kDouble)
    gLog(CLog::kWarning) << "SorterOption: " << fName
                         << " not defined as double!" << CLog::endl;
  return fDefDouble;
};

const Double_t SorterOption::GetDefaultDoubleArrayValue(const Int_t index) {
  const TString arraystring = GetDefaultStringValue();
  return GetDoubleArrayValueFromString(arraystring, index);
}

const Int_t SorterOption::GetDefaultArraySize() {
  const TString arraystring = GetDefaultStringValue();
  return GetArraySizeFromString(arraystring);
}

const char* SorterOption::GetDefaultStringValue() const {
  if (fType != kString && fType != kStringNotChecked)
    gLog(CLog::kWarning) << "SorterOption: " << fName
                         << " not defined as char*!" << CLog::endl;
  if (fDefString.IsNull())
    return (const char*)0;
  else
    return fDefString.Data();
};

const Bool_t SorterOption::GetBoolValue(const char* name) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetBoolValue();
  return kFALSE;
};

const Int_t SorterOption::GetIntValue(const char* name) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetIntValue();
  return 0;
};

const Int_t SorterOption::GetIntArrayValue(const char* name,
                                           const Int_t index) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetIntArrayValue(index);
  return 0;
};

const Double_t SorterOption::GetDoubleValue(const char* name) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetDoubleValue();
  return 0.;
};

const Double_t SorterOption::GetDoubleArrayValue(const char* name,
                                                 const Int_t index) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetDoubleArrayValue(index);
  return 0;
};

const Int_t SorterOption::GetArraySize(const char* name) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetArraySize();
  return 0;
}

const char* SorterOption::GetStringValue(const char* name) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetStringValue();
  return 0;
};

const Bool_t SorterOption::GetDefaultBoolValue(const char* name) {
  SorterOption* entry = Find(name);
  if (entry->fName == name) return entry->GetDefaultBoolValue();
  return kFALSE;
};

const Int_t SorterOption::GetDefaultIntValue(const char* name) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetDefaultIntValue();
  return 0;
};

const Int_t SorterOption::GetDefaultIntArrayValue(const char* name,
                                                  const Int_t index) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetDefaultIntArrayValue(index);
  return 0;
};

const Double_t SorterOption::GetDefaultDoubleValue(const char* name) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetDefaultDoubleValue();
  return 0.;
};

const Double_t SorterOption::GetDefaultDoubleArrayValue(const char* name,
                                                        const Int_t index) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetDefaultDoubleArrayValue(index);
  return 0;
};

const Int_t SorterOption::GetDefaultArraySize(const char* name) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetDefaultArraySize();
  return 0;
}

const char* SorterOption::GetDefaultStringValue(const char* name) {
  SorterOption* entry = Find(name);
  if (entry) return entry->GetDefaultStringValue();
  return 0;
};

void SorterOption::PrintHelp() {
  if (fgList == 0) return;
  TIter it(fgList);
  SorterOption* entry;
  while ((entry = dynamic_cast<SorterOption*>(it()))) {
    if (entry->fCmdArg == "") continue;
    gScreen << "  " << resetiosflags(std::ios::adjustfield)
            << setiosflags(std::ios::left) << std::setw(20) << entry->fCmdArg
            << entry->fHelp << resetiosflags(std::ios::adjustfield);
    switch (entry->fType) {
      case kBool:
        gScreen << " (bool)";
        break;
      case kInt:
        gScreen << " (int)";
        break;
      case kDouble:
        gScreen << " (double)";
        break;
      case kString:
      case kStringNotChecked:
        gScreen << " (char*)";
        break;
      default:
        gScreen << " (unknown type)";
        break;
    }
    gScreen << CLog::endl;
  }
};

void SorterOption::Print() {
  if (fgList == 0) return;
  gLog(CLog::kMessage) << "Current settings:" << CLog::endl;
  TIter it(fgList);
  SorterOption* entry;
  while ((entry = dynamic_cast<SorterOption*>(it()))) {
    gLog(CLog::kMessage) << "  " << resetiosflags(std::ios::adjustfield)
                         << setiosflags(std::ios::left) << std::setw(20)
                         << entry->fName;
    switch (entry->fType) {
      case kBool:
        if (entry->GetBoolValue())
          gLog(CLog::kMessage) << "kTRUE";
        else
          gLog(CLog::kMessage) << "kFALSE";
        gLog(CLog::kMessage) << " (bool)";
        break;
      case kInt:
        gLog(CLog::kMessage) << entry->GetIntValue();
        gLog(CLog::kMessage) << " (int)";
        break;
      case kDouble:
        gLog(CLog::kMessage) << entry->GetDoubleValue();
        gLog(CLog::kMessage) << " (double)";
        break;
      case kString:
      case kStringNotChecked: {
        const char* value = entry->GetStringValue();
        if (value == 0) value = "(null)";
        gLog(CLog::kMessage) << "'" << value << "'";
        gLog(CLog::kMessage) << " (char*)";
      } break;
      default:
        gLog(CLog::kMessage) << " ?? (unknown type)";
        break;
    }
    gLog(CLog::kMessage) << resetiosflags(std::ios::adjustfield) << CLog::endl;
  }
};

void SorterOption::ReadCmdLine(int argc, char** argv) {
  GetEnv();
  if (fgList == 0) return;
  TIter it(fgList);
  SorterOption* entry;
  while ((entry = dynamic_cast<SorterOption*>(it()))) {
    if (entry->fCmdArg == "") continue;
    entry->CheckCmdLine(argc, argv);
  }
  for (Int_t i = 1; i < argc; i++) {
    TString option = argv[i];
    if (option == "-h") {
      gScreen << "Usage: " << argv[0] << " [options]" << CLog::endl;
      gScreen << "  -h                  show this help" << CLog::endl;
      PrintHelp();
      exit(0);
    } else if (option == "-p") {
      Print();
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
          gScreen << "Error: extra rc path not accessible (" << extra << ")"
                  << CLog::endl;
          exit(0);
        } else {
          void* dirp = gSystem->OpenDirectory(extra);
          if (dirp == 0) {
            gLog(CLog::kMessage)
                << "Reading extra rc file: " << extra << CLog::endl;
            fgEnv->ReadFile(extra, kEnvChange);
          } else {
            if (!extra.EndsWith("/")) extra += "/";
            const char* localname = 0;
            while ((localname = gSystem->GetDirEntry(dirp))) {
              TString strName = localname;
              if (strName.EndsWith(".rc")) {
                gLog(CLog::kMessage)
                    << "Reading extra fc file: " << extra + strName
                    << CLog::endl;
                fgEnv->ReadFile(extra + strName, kEnvChange);
              }
            }
            gSystem->FreeDirectory(dirp);
          }
        }
      }
    } // end extra sorterrc
  }
};

RSEnv* SorterOption::GetEnv() {
  if (fgEnv != 0) return fgEnv;

  fgEnv = new RSEnv(".sorterrc");
  TString defaultpath = "";
  if (GetStringValue("DefaultPath")) {
    defaultpath = gSystem->ExpandPathName(GetStringValue("DefaultPath"));
    if (gSystem->AccessPathName(defaultpath)) {
      gLog(CLog::kError) << "Error: default path not accessible ("
                         << defaultpath << ")" << CLog::endl;
    } else {
      void* dirp = gSystem->OpenDirectory(defaultpath);
      if (dirp == 0) {
        gLog(CLog::kError) << "Error: default path not readable ("
                           << defaultpath << ")" << CLog::endl;
      } else {
        gLog(CLog::kSpam) << "Reading defaults from " << defaultpath
                          << CLog::endl;
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
        gLog(CLog::kError) << "Error: rc-file not readable (" << filename << ")"
                           << CLog::endl;
      } else {
        void* dirp = gSystem->OpenDirectory(filename);
        if (dirp == 0) {
          gLog(CLog::kSpam) << "Reading " << filename << CLog::endl;
          fgEnv->ReadFile(filename, kEnvUser);
        } else {
          if (!filename.EndsWith("/")) filename += "/";
          const char* localname = 0;
          while ((localname = gSystem->GetDirEntry(dirp))) {
            TString strName = localname;
            if (strName.EndsWith(".rc")) {
              gLog(CLog::kSpam)
                  << "Reading " << filename + strName << CLog::endl;
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

const char* SorterOption::Getvalue(const char* name) const {
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
    // gScreen << "Searching " << to_be_checked << CLog::endl;
    tmp = GetEnv()->Lookup(to_be_checked);
    if (tmp) {
      // gScreen << "Found." << CLog::endl;
      delete components;
      return tmp->GetValue();
    }
    count += 2;
  }
  delete components;
  return 0;
}

// code taken from TEnv functions

static struct BoolNameTable_t {
  const char* fName;
  Int_t fValue;
} gBoolNames[] = {{"TRUE", 1}, {"FALSE", 0}, {"ON", 1},  {"OFF", 0}, {"YES", 1},
                  {"NO", 0},   {"OK", 1},    {"NOT", 0}, {0, 0}};

//______________________________________________________________________________
Int_t SorterOption::GetValue(const char* name, Int_t dflt) const {
  // Returns the integer value for a resource. If the resource is not found
  // return the dflt value.

  const char* cp = SorterOption::Getvalue(name);
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
Double_t SorterOption::GetValue(const char* name, Double_t dflt) const {
  // Returns the double value for a resource. If the resource is not found
  // return the dflt value.

  const char* cp = SorterOption::Getvalue(name);
  if (cp) {
    char* endptr;
    Double_t val = strtod(cp, &endptr);
    if (fpt_eq(val, 0.0) && cp == endptr) return dflt;
    return val;
  }
  return dflt;
}

//______________________________________________________________________________
const char* SorterOption::GetValue(const char* name, const char* dflt) const {
  // Returns the character value for a named resouce. If the resource is
  // not found the dflt value is returned.

  const char* cp = SorterOption::Getvalue(name);
  if (cp) return cp;
  return dflt;
}
