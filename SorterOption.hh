/******************************************************************************
 *
 * $Id$d
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
  \file   SorterOption.hh
  \brief  Defines options, which can be set by command line, .sorterrc and
          default hard-coded value

  <long description>

  \author Volker Hejny
  \date   2002-07-29
*/

#ifndef _SORTEROPTION_HH
#define _SORTEROPTION_HH

#include "TObject.h"
#include "TString.h"

class TList;
class RSEnv;

class SorterOption : public TObject {
public:
  enum OptionType { kNone, kBool, kInt, kDouble, kString, kStringNotChecked };

  SorterOption();
  SorterOption(const char* name, const char* cmd, const char* help,
               Bool_t defval, void (*f)() = 0);
  SorterOption(const char* name, const char* cmd, const char* help,
               Int_t defval, void (*f)() = 0);
  SorterOption(const char* name, const char* cmd, const char* help,
               Double_t defval, void (*f)() = 0);
  SorterOption(const char* name, const char* cmd, const char* help,
               const char* defval, void (*f)() = 0);
  SorterOption(const char* name, Bool_t defval);
  SorterOption(const char* name, Int_t defval);
  SorterOption(const char* name, Double_t defval);
  SorterOption(const char* name, const char* defval);
  virtual ~SorterOption();

  SorterOption* Expand(TObject* obj);
  SorterOption* Expand(const TString& s1, const TString& s2);

  void CheckCmdLine(int argc, char** argv);
  const char* GetHelp() const;

  const Bool_t GetBoolValue() const;
  const Int_t GetIntValue() const;
  const Int_t GetIntArrayValue(const Int_t index);
  const Double_t GetDoubleValue() const;
  const Double_t GetDoubleArrayValue(const Int_t index);
  const Int_t GetArraySize();
  const char* GetStringValue();

  const Bool_t GetDefaultBoolValue() const;
  const Int_t GetDefaultIntValue() const;
  const Int_t GetDefaultIntArrayValue(const Int_t index);

  const Double_t GetDefaultDoubleValue() const;
  const Double_t GetDefaultDoubleArrayValue(const Int_t index);
  const Int_t GetDefaultArraySize();

  const char* GetDefaultStringValue() const;

  static const Bool_t GetBoolValue(const char* name);
  static const Int_t GetIntValue(const char* name);
  static const Int_t GetIntArrayValue(const char* name, const Int_t index);
  static const Double_t GetDoubleValue(const char* name);
  static const Double_t GetDoubleArrayValue(const char* name,
                                            const Int_t index);
  static const Int_t GetArraySize(const char* name);
  static const char* GetStringValue(const char* name);

  static const Bool_t GetDefaultBoolValue(const char* name);
  static const Int_t GetDefaultIntValue(const char* name);
  static const Int_t GetDefaultIntArrayValue(const char* name,
                                             const Int_t index);
  static const Double_t GetDefaultDoubleValue(const char* name);
  static const Double_t GetDefaultDoubleArrayValue(const char* name,
                                                   const Int_t index);
  static const Int_t GetDefaultArraySize(const char* name);
  static const char* GetDefaultStringValue(const char* name);

  static RSEnv* GetEnv();
  static void ReadCmdLine(int argc, char** argv);
  static void PrintHelp();
  static void Print();

  static SorterOption* Find(const char* name);

private:
  SorterOption(const SorterOption& ref);

  void Init(const char* name, const char* cmd, const char* help);
  Int_t GetValue(const char* name, Int_t def) const;
  Double_t GetValue(const char* name, Double_t def) const;
  const char* GetValue(const char* name, const char* def) const;
  const char* Getvalue(const char* name) const;

  static const Int_t GetIntArrayValueFromString(const TString arraystring,
                                                const Int_t index);
  static const Double_t GetDoubleArrayValueFromString(const TString arraystring,
                                                      const Int_t index);
  static const Int_t GetArraySizeFromString(const TString arraystring);
  TString fName;   // name used in .sorterrc
  TString fCmdArg; // name for command line
  TString fHelp;   // help text

  Int_t fDefInt; // default values;
  Double_t fDefDouble;
  TString fDefString;
  OptionType fType;

  void (*fFunction)(); // function to be called when changed

  static TList* fgList; // list of command line options
  static RSEnv* fgEnv;  // general rootsorter environment

  ClassDef(SorterOption, 0)
};

#endif
