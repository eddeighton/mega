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

    features = {}
    commands = []
    structs = []
    structExtends = {}

    def calculateTypeName(self, isConst, refType, typeName):
        name = ""
        if isConst:
            name = "const "

        keywords = []
        # keywords = ["Device", "Instance"]

        if typeName.startswith("Vk"):
            typeName = typeName.removeprefix("Vk")
            #for keyword in keywords:
            #    if typeName.startswith(keyword):
            #        typeName = typeName.removeprefix(keyword)
            name = "vk::" + typeName

        else:
            name += typeName

        if refType == 1:
            name += "*"
        elif refType == 2:
            name += "**"
        return name

    def enumStructExtensionChains(self, structName):
        result = [[self.calculateTypeName(False, 0, structName)]]
        if structName in self.structExtends:
            for next in self.structExtends[structName]:
                for tail in self.enumStructExtensionChains(next):
                    result.append([self.calculateTypeName(False, 0, structName)] + tail)
        return result

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.may_alias = None
        self.features["features"] = []

    def beginFile(self, genOpts):
        OutputGenerator.beginFile(self, genOpts)

    def endFile(self):
        # C-specific
        # Finish C++ wrapper and multiple inclusion protection
        self.newline()

        json_data = {}
        json_data["chainTraits"] = []

        typeID = 1
        for i, (struct, attr) in enumerate(self.structExtends.items()):
            for chain in self.enumStructExtensionChains(struct):
                json_data["chainTraits"].append({"id": typeID, "types": chain})
                typeID = typeID + 1

        json_data["chainCtors"] = []
        json_data["nonChainCtors"] = []
        json_data["commands"] = []

        basic = self.features["VK_VERSION_1_0"]
        for struct in basic["structs"]:

            if struct["has_pnext"]:

                params = []
                param_index = 0
                for member in struct["members"]:

                    if member["is_len"]:
                        continue
                    if member["name"] == "sType":
                        continue

                    isChainType = False
                    memberType = ""
                    if member["name"] == "pNext":
                        isChainType = True
                        memberType = "TChainTail"
                    else:
                        memberType = self.calculateTypeName(
                            member["const"], member["ref"], member["type"]
                        )

                    params.append(
                        {
                            "chain": isChainType,
                            "index": param_index,
                            "type": memberType,
                            "name": member["name"],
                        }
                    )
                    param_index = param_index + 1

                json_data["chainCtors"].append(
                    {
                        "type": self.calculateTypeName(False, 0, struct["name"]),
                        "name": "make_" + struct["name"],
                        "params": params,
                        "param_count" : len( params )
                    }
                )
            else:

                params = []
                param_index = 0
                for member in struct["members"]:

                    if member["is_len"]:
                        continue
                    if member["name"] == "sType":
                        continue

                    isChainType = False
                    memberType = self.calculateTypeName(
                        member["const"], member["ref"], member["type"]
                    )

                    params.append(
                        {
                            "index": param_index,
                            "type": memberType,
                            "name": member["name"],
                        }
                    )
                    param_index = param_index + 1

                json_data["nonChainCtors"].append(
                    {
                        "type": self.calculateTypeName(False, 0, struct["name"]),
                        "name": "make_" + struct["name"],
                        "params": params,
                        "param_count" : len( params )
                    }
                )

        for command in basic["commands"]:
            params = []
            param_index = 0
            for parameter in command["parameters"]:

                if parameter["is_len"]:
                    continue

                isChainType = False

                parameterType = self.calculateTypeName(
                    parameter["const"], parameter["ref"], parameter["type"]
                )

                params.append(
                    {
                        "index": param_index,
                        "type": parameterType,
                        "name": parameter["name"],
                    }
                )
                param_index = param_index + 1

            commandResult = "void"
            hasResult = False

            if command["return"] != "void":
                commandResult = (
                    "Lazy< "
                    + self.calculateTypeName(False, 0, command["return"])
                    + " >"
                )
                hasResult = True

            json_data["commands"].append(
                {
                    "type": commandResult,
                    "has_result": hasResult,
                    "name": command["name"],
                    "params": params,
                    "param_count" : len( params )
                }
            )

        write(json.dumps(json_data), file=self.outFile)
        OutputGenerator.endFile(self)

    def beginFeature(self, interface, emit):
        # Start processing in superclass
        OutputGenerator.beginFeature(self, interface, emit)
        self.commands = []
        self.structs = []

    def endFeature(self):
        "Actually write the interface to the output file."
        
        self.features[self.featureName] = {
            "name": self.featureName,
            "commands": self.commands,
            "structs": self.structs,
        }
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
        
        ignoredStructs = [ "VkBaseInStructure", "VkPipelineRobustnessCreateInfoEXT" ]
        if typeName in ignoredStructs:
            return

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
                # attr is comma delimited list
                #if "VK_VERSION_1_0" == self.featureName:
                for base in attr.split(","):
                    if base not in self.structExtends:
                        self.structExtends[base] = []
                    self.structExtends[base].append(typeName)
            elif key == "returnedonly":
                pass
            elif key == "comment":
                pass
            else:
                raise Exception("Unknown struct attribute: " + key)

        members = []
        has_pnext = False
        length_names = []

        for member in typeinfo.elem.findall(".//member"):
            param_type = ""
            param_name = ""
            param_const = False
            param_ref = 0
            param_optional = 0
            param_len = []

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
                    param_len = attr.split(",")
                    for length_name in param_len:
                        length_names.append(length_name)
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
                            param_ref = 2
                        elif elem.tail.strip() == "* const *":
                            param_ref = 2
                        elif elem.tail.strip() != "":
                            raise Exception("Unknown member element tail: " + elem.tail)
                elif elem.tag == "name":
                    param_name = elem.text
                    if param_name == "pNext":
                        has_pnext = True

            members.append(
                {
                    "type": param_type,
                    "name": param_name,
                    "const": param_const,
                    "ref": param_ref,
                    "opt": param_optional,
                    "len": param_len,
                    "is_len": False,
                }
            )

        for member in members:
            if member["name"] in length_names:
                member["is_len"] = True

        self.structs.append(
            {"name": typeName, "members": members, "has_pnext": has_pnext}
        )

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
                raise Exception("Unknown command proto element: " + elem.tag)

        command_parameters = []
        length_names = []
        for param in params:
            param_type = ""
            param_name = ""
            param_const = False
            param_ref = 0
            param_optional = 0
            param_len = []

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
                    param_len = attr.split(",")
                    for length_name in param_len:
                        length_names.append(length_name)
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

            command_parameters.append(
                {
                    "type": param_type,
                    "name": param_name,
                    "const": param_const,
                    "ref": param_ref,
                    "opt": param_optional,
                    "len": param_len,
                    "is_len": False,
                }
            )

        for parameter in command_parameters:
            if parameter["name"] in length_names:
                parameter["is_len"] = True

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
