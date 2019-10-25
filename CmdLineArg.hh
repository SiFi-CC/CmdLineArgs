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
  \file   CmdLineArg.hh
  \brief  Defines options, which can be set by command line, .sorterrc and
          default hard-coded value

  <long description>

  \author Volker Hejny
  \date   2002-07-29
*/

#ifndef _CMDLINEARG_HH
#define _CMDLINEARG_HH

#include "TObject.h"
#include "TString.h"

#include <vector>

class TList;
class TEnv;

class CmdLineArg : public TObject {
public:
  enum OptionType {
    kNone,
    kFlag,
    kBool,
    kInt,
    kDouble,
    kString,
    kStringNotChecked
  };

  CmdLineArg();
  CmdLineArg(const char* name, const char* help, OptionType type,
             void (*f)() = nullptr, bool greedy = false);

  virtual ~CmdLineArg();

  CmdLineArg* Expand(TObject* obj);
  CmdLineArg* Expand(const TString& s1, const TString& s2);

  const char* GetHelp() const;

  const Bool_t GetFlagValue() const;
  const Bool_t GetBoolValue() const;
  const Int_t GetIntValue() const;
  const Int_t GetIntArrayValue(const Int_t index);
  const Double_t GetDoubleValue() const;
  const Double_t GetDoubleArrayValue(const Int_t index);
  const Int_t GetArraySize();
  const char* GetStringValue(Bool_t arrayParsing = kFALSE);

  static const Bool_t GetFlagValue(const char* name);
  static const Bool_t GetBoolValue(const char* name);
  static const Int_t GetIntValue(const char* name);
  static const Int_t GetIntArrayValue(const char* name, const Int_t index);
  static const Double_t GetDoubleValue(const char* name);
  static const Double_t GetDoubleArrayValue(const char* name,
                                            const Int_t index);
  static const Int_t GetArraySize(const char* name);
  static const char* GetStringValue(const char* name);

  void PrintHelp(const char* placeholder = nullptr);
  void Print();

private:
  CmdLineArg(const char* name, OptionType type);
  CmdLineArg(const CmdLineArg& ref); // LCOV_EXCL_LINE

  void Init(const char* name, const char* help, bool greedy = false);
  Int_t GetValue(const char* name, Int_t def) const;
  Double_t GetValue(const char* name, Double_t def) const;
  const char* GetValue(const char* name, const char* def) const;
  const char* Getvalue(const char* name) const;

  static const Int_t GetIntArrayValueFromString(const TString arraystring,
                                                const Int_t index);
  static const Double_t GetDoubleArrayValueFromString(const TString arraystring,
                                                      const Int_t index);
  static const Int_t GetArraySizeFromString(const TString arraystring);

public:
  TString fName; // name used in .sorterrc
  TString fHelp; // help text
  TString fValue;

  OptionType fType;

  void (*fFunction)(); // function to be called when changed

  static const TString delim;

  friend class CmdLineConfig;

  ClassDef(CmdLineArg, 0); // LCOV_EXCL_LINE
};

#endif
