#!/usr/bin/python3 -i
#
# Copyright 2013-2022 The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0

import os
import re
import json

from collections import defaultdict
from generator import GeneratorOptions, OutputGenerator, noneStr, regSortFeatures, write


class SSAJSONGeneratorOptions(GeneratorOptions):
    """SSAJSONGeneratorOptions - subclass of GeneratorOptions.

    Adds options used by SSAJSONGenerator objects during C language header
    generation."""

    def __init__(
        self,
        prefixText="",
        genFuncPointers=True,
        protectFile=True,
        protectFeature=True,
        protectProto=None,
        protectProtoStr=None,
        apicall="",
        apientry="",
        apientryp="",
        indentFuncProto=True,
        indentFuncPointer=False,
        alignFuncParam=0,
        genEnumBeginEndRange=False,
        genAliasMacro=False,
        aliasMacro="",
        misracstyle=False,
        misracppstyle=False,
        **kwargs
    ):
        """Constructor.
        Additional parameters beyond parent class:

        - prefixText - list of strings to prefix generated header with
        (usually a copyright statement + calling convention macros).
        - protectFile - True if multiple inclusion protection should be
        generated (based on the filename) around the entire header.
        - protectFeature - True if #ifndef..#endif protection should be
        generated around a feature interface in the header file.
        - genFuncPointers - True if function pointer typedefs should be
        generated
        - protectProto - If conditional protection should be generated
        around prototype declarations, set to either '#ifdef'
        to require opt-in (#ifdef protectProtoStr) or '#ifndef'
        to require opt-out (#ifndef protectProtoStr). Otherwise
        set to None.
        - protectProtoStr - #ifdef/#ifndef symbol to use around prototype
        declarations, if protectProto is set
        - apicall - string to use for the function declaration prefix,
        such as APICALL on Windows.
        - apientry - string to use for the calling convention macro,
        in typedefs, such as APIENTRY.
        - apientryp - string to use for the calling convention macro
        in function pointer typedefs, such as APIENTRYP.
        - indentFuncProto - True if prototype declarations should put each
        parameter on a separate line
        - indentFuncPointer - True if typedefed function pointers should put each
        parameter on a separate line
        - alignFuncParam - if nonzero and parameters are being put on a
        separate line, align parameter names at the specified column
        - genEnumBeginEndRange - True if BEGIN_RANGE / END_RANGE macros should
        be generated for enumerated types
        - genAliasMacro - True if the OpenXR alias macro should be generated
        for aliased types (unclear what other circumstances this is useful)
        - aliasMacro - alias macro to inject when genAliasMacro is True
        - misracstyle - generate MISRA C-friendly headers
        - misracppstyle - generate MISRA C++-friendly headers"""

        GeneratorOptions.__init__(self, **kwargs)

        self.prefixText = prefixText
        """list of strings to prefix generated header with (usually a copyright statement + calling convention macros)."""

        self.genFuncPointers = genFuncPointers
        """True if function pointer typedefs should be generated"""

        self.protectFile = protectFile
        """True if multiple inclusion protection should be generated (based on the filename) around the entire header."""

        self.protectFeature = protectFeature
        """True if #ifndef..#endif protection should be generated around a feature interface in the header file."""

        self.protectProto = protectProto
        """If conditional protection should be generated around prototype declarations, set to either '#ifdef' to require opt-in (#ifdef protectProtoStr) or '#ifndef' to require opt-out (#ifndef protectProtoStr). Otherwise set to None."""

        self.protectProtoStr = protectProtoStr
        """#ifdef/#ifndef symbol to use around prototype declarations, if protectProto is set"""

        self.apicall = apicall
        """string to use for the function declaration prefix, such as APICALL on Windows."""

        self.apientry = apientry
        """string to use for the calling convention macro, in typedefs, such as APIENTRY."""

        self.apientryp = apientryp
        """string to use for the calling convention macro in function pointer typedefs, such as APIENTRYP."""

        self.indentFuncProto = indentFuncProto
        """True if prototype declarations should put each parameter on a separate line"""

        self.indentFuncPointer = indentFuncPointer
        """True if typedefed function pointers should put each parameter on a separate line"""

        self.alignFuncParam = alignFuncParam
        """if nonzero and parameters are being put on a separate line, align parameter names at the specified column"""

        self.genEnumBeginEndRange = genEnumBeginEndRange
        """True if BEGIN_RANGE / END_RANGE macros should be generated for enumerated types"""

        self.genAliasMacro = genAliasMacro
        """True if the OpenXR alias macro should be generated for aliased types (unclear what other circumstances this is useful)"""

        self.aliasMacro = aliasMacro
        """alias macro to inject when genAliasMacro is True"""

        self.misracstyle = misracstyle
        """generate MISRA C-friendly headers"""

        self.misracppstyle = misracppstyle
        """generate MISRA C++-friendly headers"""

        self.codeGenerator = True
        """True if this generator makes compilable code"""


class SSAJSONGenerator(OutputGenerator):
    """Generates C-language API interfaces."""

    json_data = {}
    commands = []
    structs = []

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.may_alias = None
        self.json_data["features"] = []

    def beginFile(self, genOpts):
        OutputGenerator.beginFile(self, genOpts)

    def endFile(self):
        # C-specific
        # Finish C++ wrapper and multiple inclusion protection
        self.newline()
        write(json.dumps(self.json_data), file=self.outFile)
        OutputGenerator.endFile(self)

    def beginFeature(self, interface, emit):
        # Start processing in superclass
        OutputGenerator.beginFeature(self, interface, emit)
        self.commands = []
        self.structs = []

    def endFeature(self):
        "Actually write the interface to the output file."

        self.json_data["features"].append(
            {
                "name": self.featureName,
                "commands": self.commands,
                "structs": self.structs,
            }
        )
        OutputGenerator.endFeature(self)

    def genType(self, typeinfo, name, alias):
        "Generate type."
        OutputGenerator.genType(self, typeinfo, name, alias)
        typeElem = typeinfo.elem
        category = typeElem.get("category")
        if category == "funcpointer":
            section = "struct"
        else:
            section = category
        if category in ("struct", "union"):
            # If the type is a struct type, generate it using the
            # special-purpose generator.
            self.genStruct(typeinfo, name, alias)

    def genProtectString(self, protect_str):
        pass

    def typeMayAlias(self, typeName):
        if not self.may_alias:
            # First time we have asked if a type may alias.
            # So, populate the set of all names of types that may.

            # Everyone with an explicit mayalias="true"
            self.may_alias = set(
                typeName
                for typeName, data in self.registry.typedict.items()
                if data.elem.get("mayalias") == "true"
            )

            # Every type mentioned in some other type's parentstruct attribute.
            parent_structs = (
                otherType.elem.get("parentstruct")
                for otherType in self.registry.typedict.values()
            )
            self.may_alias.update(set(x for x in parent_structs if x is not None))
        return typeName in self.may_alias

    def genStruct(self, typeinfo, typeName, alias):

        for i, (key, attr) in enumerate(typeinfo.elem.attrib.items()):
            if key == "category":
                pass
            elif key == "name":
                pass
            elif key == "alias":
                pass
            elif key == "allowduplicate":
                pass
            elif key == "structextends":
                pass
            elif key == "returnedonly":
                pass
            elif key == "comment":
                pass
            else:
                raise Exception("Unknown struct attribute: " + key)

        members = {}

        for member in typeinfo.elem.findall(".//member"):
            param_type = ""
            param_name = ""
            param_const = False
            param_ref = 0
            param_optional = 0

            if member.text == "*":
                param_ref = True
            elif member.text == "const ":
                param_const = True
            for i, (key, attr) in enumerate(member.attrib.items()):
                if key == "optional":
                    if attr == "false":
                        param_optional = 0
                    elif attr == "true":
                        param_optional = 1
                    elif attr == "false,true":
                        param_optional = 2
                    elif attr == "true,false":
                        param_optional = 2
                    else:
                        raise Exception("Unknown optionality: " + attr)
                elif key == "values":
                    pass
                elif key == "noautovalidity":
                    pass
                elif key == "len":
                    pass
                elif key == "altlen":
                    pass
                elif key == "externsync":
                    pass
                elif key == "selection":
                    pass
                elif key == "limittype":
                    pass
                elif key == "selector":
                    pass
                elif key == "objecttype":
                    pass
                else:
                    raise Exception("Unknown struct member attrib: " + key)

            for elem in member:
                if elem.tag == "type":
                    param_type = elem.text
                    if elem.tail:
                        if elem.tail.strip() == "*":
                            param_ref = 1
                        elif elem.tail.strip() == "* const*":
                            param_ref = 1
                        elif elem.tail.strip() == "* const *":
                            param_ref = 1
                        elif elem.tail.strip() != "":
                            raise Exception("Unknown member element tail: " + elem.tail)
                elif elem.tag == "name":
                    param_name = elem.text

            members[param_name] = {
                "type": param_type,
                "name": param_name,
                "const": param_const,
                "ref": param_ref,
                "opt": param_optional,
            }

        self.structs.append({"name": typeName, "members": members})

    def genGroup(self, groupinfo, groupName, alias=None):
        pass

    def genEnum(self, enuminfo, name, alias):
        pass

    def genCmd(self, cmdinfo, name, alias):
        "Command generation"

        proto = cmdinfo.elem.find("proto")
        params = cmdinfo.elem.findall("param")

        command_name = ""
        command_return = ""

        for i, (key, attr) in enumerate(cmdinfo.elem.attrib.items()):
            if key == "queues":
                pass
            elif key == "renderpass":
                pass
            elif key == "cmdbufferlevel":
                pass
            elif key == "successcodes":
                pass
            elif key == "errorcodes":
                pass
            elif key == "name":
                pass
            elif key == "alias":
                pass
            elif key == "comment":
                pass
            else:
                raise Exception("Unknown command attribute: " + key)

        for elem in proto:
            if elem.tag == "name":
                command_name = elem.text
            elif elem.tag == "type":
                command_return = elem.text
            else:
                raise Excetion( "Unknown command proto element: " + elem.tag )

        command_parameters = {}
        for param in params:
            param_type = ""
            param_name = ""
            param_const = False
            param_ref = 0
            param_optional = 0

            if param.text == "*":
                param_ref = True
            elif param.text == "const ":
                param_const = True
            for i, (key, attr) in enumerate(param.attrib.items()):
                if key == "optional":
                    if attr == "false":
                        param_optional = 0
                    elif attr == "true":
                        param_optional = 1
                    elif attr == "false,true":
                        param_optional = 2
                    else:
                        raise Exception("Unknown optionality: " + attr)
                elif key == "externsync":
                    pass
                elif key == "len":
                    pass
                elif key == "noautovalidity":
                    pass
                elif key == "objecttype":
                    pass
                elif key == "validstructs":
                    pass
                else:
                    raise Exception("Unknown command param attrib: " + key)

            for elem in param:
                if elem.tag == "type":
                    param_type = elem.text
                    if elem.tail.strip() == "*":
                        param_ref = 1
                    elif elem.tail.strip() == "**":
                        param_ref = 2
                    elif elem.tail.strip() == "* const*":
                        param_ref = 3
                    elif elem.tail.strip() != "":
                        raise Exception("Unknown param element tail: " + elem.tail)
                elif elem.tag == "name":
                    param_name = elem.text

            command_parameters[param_name] = {
                "type": param_type,
                "name": param_name,
                "const": param_const,
                "ref": param_ref,
                "opt": param_optional,
            }

        self.commands.append(
            {
                "name": command_name,
                "return": command_return,
                "parameters": command_parameters,
            }
        )

    def misracstyle(self):
        return self.genOpts.misracstyle

    def misracppstyle(self):
        return self.genOpts.misracppstyle
