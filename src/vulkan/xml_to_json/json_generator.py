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

    # features = {}
    commands = []
    structs = []
    structExtends = {}

    def calculateTypeName(self, isConst, refType, typeName):
        name = ""
        if isConst:
            name = "const "

        removedRefTypes = [
            "VkDescriptorSetAllocateInfo",
            "VkCommandBufferAllocateInfo",
            "VkClearColorValue",
            "VkClearDepthStencilValue",
            "VkImageSparseMemoryRequirementsInfo2",
            "VkPhysicalDeviceSparseImageFormatInfo2",
            "VkDeviceImageMemoryRequirements",
            "VkPhysicalDeviceSurfaceInfo2KHR",
            "VkPipelineInfoKHR",
            "VkPipelineExecutableInfoKHR",
            "VkPipelineExecutableInfoKHR",
        ]
        if typeName in removedRefTypes:
            if refType == 1:
                refType = 0

        if typeName.startswith("Vk"):
            typeName = typeName.removeprefix("Vk")
            name = "vk::" + typeName
        else:
            name += typeName

        if refType == 1:
            name += "*"
        elif refType == 2:
            name += "**"
        return name

    def calculateCmdName(self, cmdName, asMemberAccess, memberRemoval):
        name = ""
        keywords = []
        # keywords = ["Device", "Instance"]

        if cmdName.startswith("vk"):
            cmdName = cmdName.removeprefix("vk")
            for keyword in keywords:
                if cmdName.startswith(keyword):
                    cmdName = cmdName.removeprefix(keyword)
            if asMemberAccess:
                name = cmdName
                for removal in memberRemoval:
                    name = name.replace(removal, "")
                name = name[0].lower() + name[1:]

                specialTemplates = {
                    "getQueryPoolResults": "getQueryPoolResults< uint8_t >",
                    "updateBuffer": "updateBuffer< uint8_t >",
                    "pushConstants": "pushConstants< uint8_t >",
                    "getRayTracingShaderGroupHandlesKHR": "getRayTracingShaderGroupHandlesKHR< uint8_t >",
                    "getRayTracingShaderGroupHandlesNV": "getRayTracingShaderGroupHandlesNV< uint8_t >",
                    "getAccelerationStructureHandleNV": "getAccelerationStructureHandleNV< uint8_t >",
                }
                if name in specialTemplates:
                    name = specialTemplates[name]

            else:
                cmdName = cmdName[0].lower() + cmdName[1:]
                name = "vk::" + cmdName

        else:
            name += cmdName
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
        # self.features["features"] = []

    def beginFile(self, genOpts):
        OutputGenerator.beginFile(self, genOpts)

    def endFile(self):
        # C-specific
        # Finish C++ wrapper and multiple inclusion protection
        self.newline()

        # basic = self.features["VK_VERSION_1_0"]

        json_data = {}
        json_data["chainTraits"] = []

        # ensure ALL chainable structs in in the mapping even if they
        # have no actual extensions
        for struct in self.structs:
            if struct["has_pnext"]:
                structName = struct["name"]
                if structName not in self.structExtends:
                    self.structExtends[structName] = []

        ignoreStructDupes = [
            "VkImageMemoryBarrier2KHR",
            "VkBufferImageCopy2KHR",
            "VkImageBlit2KHR",
        ]

        typeID = 1
        for i, (struct, attr) in enumerate(self.structExtends.items()):
            if struct in ignoreStructDupes:
                continue
            for chain in self.enumStructExtensionChains(struct):
                json_data["chainTraits"].append({"id": typeID, "types": chain})
                typeID = typeID + 1

        json_data["chainCtors"] = []
        json_data["nonChainCtors"] = []
        json_data["commands"] = []

        for struct in self.structs:

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
                    elif member["is_array"]:
                        if member["type"] == "void":
                            memberType = "std::vector< char >"
                        elif member["type"] == "char":
                            memberType = "const std::string"
                        else:
                            memberType = "std::vector< " + member["type"] + " >"
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
                            "optional": member["opt"] > 0,
                        }
                    )
                    param_index = param_index + 1

                json_data["chainCtors"].append(
                    {
                        "type": self.calculateTypeName(False, 0, struct["name"]),
                        "name": "make_" + struct["name"],
                        "params": params,
                        "param_count": len(params),
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
                    if member["is_array"]:
                        if member["type"] == "void":
                            memberType = "std::vector< char >"
                        else:
                            memberType = "std::vector< " + member["type"] + " >"
                    else:
                        memberType = self.calculateTypeName(
                            member["const"], member["ref"], member["type"]
                        )

                    params.append(
                        {
                            "index": param_index,
                            "type": memberType,
                            "name": member["name"],
                            "optional": member["opt"] > 0,
                        }
                    )
                    param_index = param_index + 1

                json_data["nonChainCtors"].append(
                    {
                        "type": self.calculateTypeName(False, 0, struct["name"]),
                        "name": "make_" + struct["name"],
                        "params": params,
                        "param_count": len(params),
                        "typeid": typeID,
                    }
                )
                typeID = typeID + 1

        for command in self.commands:
            params = []
            results = []
            param_index = 0
            result_index = 0
            interp = []
            isMemberCall = False
            memberType = ""
            memberAccess = ""
            memberRemoval = []
            isFirstParam = True
            lastResultIsArray = False
            lastResultIndex = 0
            for parameter in command["parameters"]:

                if isFirstParam:
                    isFirstParam = False

                    if parameter["type"] == "VkInstance":
                        isMemberCall = True
                        memberAccess = "parameters[ " + str(param_index) + "]"
                        memberType = "vk::Instance"
                        memberRemoval = ["Instance"]
                        param_index = param_index + 1
                        continue

                    if parameter["type"] == "VkPhysicalDevice":
                        isMemberCall = True
                        memberAccess = "parameters[ " + str(param_index) + "]"
                        memberType = "vk::PhysicalDevice"
                        memberRemoval = ["PhysicalDevice"]
                        param_index = param_index + 1
                        continue

                    if parameter["type"] == "VkDevice":
                        isMemberCall = True
                        memberAccess = "parameters[ " + str(param_index) + "]"
                        memberType = "vk::Device"
                        memberRemoval = ["Device"]
                        param_index = param_index + 1
                        continue

                    if parameter["type"] == "VkQueue":
                        isMemberCall = True
                        memberAccess = "parameters[ " + str(param_index) + "]"
                        memberType = "vk::Queue"
                        memberRemoval = ["Queue"]
                        param_index = param_index + 1
                        continue

                    if parameter["type"] == "VkCommandBuffer":
                        isMemberCall = True
                        memberAccess = "parameters[ " + str(param_index) + "]"
                        memberType = "vk::CommandBuffer"
                        memberRemoval = ["CommandBuffer", "Cmd"]
                        param_index = param_index + 1
                        continue

                dataSizeExemptions = [
                    "vkGetQueryPoolResults",
                    "vkGetRayTracingShaderGroupHandlesKHR",
                    "vkGetRayTracingShaderGroupHandlesNV",
                    "vkGetAccelerationStructureHandleNV",
                ]

                if parameter["is_len"]:
                    if command["name"] not in dataSizeExemptions:
                        continue

                if parameter["is_array"]:
                    if parameter["type"] == "void":
                        parameterType = "std::vector< uint8_t >"
                    elif parameter["type"] == "char":
                        parameterType = "const std::string"
                    else:
                        parameterType = (
                            "std::vector< "
                            + self.calculateTypeName(False, 0, parameter["type"])
                            + " >"
                        )
                else:
                    parameterType = self.calculateTypeName(
                        parameter["const"], parameter["ref"], parameter["type"]
                    )

                is_result = False

                if parameter["ref"] == 1:
                    if not parameter["const"]:
                        is_result = True

                if is_result:
                    if parameter["is_array"]:
                        lastResultIsArray = True
                        lastResultIndex = len(interp)
                    else:
                        lastResultIsArray = False
                    results.append(
                        {
                            "index": result_index,
                            "type": parameterType,
                            "name": parameter["name"],
                            "optional": False,  # parameter["opt"] > 0,
                        }
                    )
                    interp.append(
                        {
                            "type": parameterType,
                            "optional": False,  # parameter["opt"] > 0,
                            "access": "results[ " + str(result_index) + "]",
                            "is_c_array": parameter["is_c_array"],
                        }
                    )
                    result_index = result_index + 1
                else:
                    params.append(
                        {
                            "index": param_index,
                            "type": parameterType,
                            "name": parameter["name"],
                            "optional": False,  # parameter["opt"] > 0,
                        }
                    )
                    interp.append(
                        {
                            "type": parameterType,
                            "optional": False,  # parameter["opt"] > 0,
                            "access": "parameters[ " + str(param_index) + "]",
                            "is_c_array": parameter["is_c_array"],
                        }
                    )
                    param_index = param_index + 1

            lastResult = {}
            if lastResultIsArray:
                lastResult = interp[lastResultIndex]
                results = results[:-1]
                interp = interp[:lastResultIndex] + interp[lastResultIndex + 1 :]

            mainResultType = "Lazy< Void >"
            hasMainResult = False

            stuffThatVulkanHPPTurnsToVoidReturnType = [
                "vkQueueSubmit",
                "vkQueueWaitIdle",
                "vkFlushMappedMemoryRanges",
                "vkInvalidateMappedMemoryRanges",
                "vkBindBufferMemory",
                "vkBindImageMemory",
                "vkQueueBindSparse",
                "vkResetFences",
                "vkSetEvent",
                "vkResetEvent",
                "vkDeviceWaitIdle",
                "vkMergePipelineCaches",
                "vkResetDescriptorPool",
                "vkAllocateDescriptorSets",
                "vkFreeDescriptorSets",
                "vkResetCommandPool",
                "vkEndCommandBuffer",
                "vkResetCommandBuffer",
                "vkBindBufferMemory2",
                "vkBindImageMemory2",
                "vkSetPrivateData",
                "vkQueueSubmit2",
                "vkBindBufferMemory2KHR",
                "vkBindImageMemory2KHR",
                "vkQueueSubmit2KHR",
                "vkReleaseDisplayEXT",
                "vkMergeValidationCachesEXT",
                "vkBindAccelerationStructureMemoryNV",
                "vkCompileDeferredNV",
                "vkReleasePerformanceConfigurationINTEL",
                "vkQueueSetPerformanceConfigurationINTEL",
                "vkAcquireDrmDisplayEXT",
                "vkSetPrivateDataEXT",
                "vkAcquireWinrtDisplayNV"
            ]

            if command["name"] in stuffThatVulkanHPPTurnsToVoidReturnType:
                pass
            elif command["return"] != "void":
                mainResultType = (
                    "Lazy< "
                    + self.calculateTypeName(False, 0, command["return"])
                    + " >"
                )
                hasMainResult = True

            commandResultType = "std::tuple< " + mainResultType
            if len(results) > 0:
                for result in results:
                    commandResultType = (
                        commandResultType + ", Lazy< " + result["type"] + " >"
                    )
            commandResultType = commandResultType + " >"

            json_data["commands"].append(
                {
                    "result_type": commandResultType,
                    "main_result": mainResultType,
                    "has_main_result": hasMainResult,
                    "name": command["name"],
                    "is_member_call": isMemberCall,
                    "member_type": memberType,
                    "member_access": memberAccess,
                    "vkhpp_name": self.calculateCmdName(
                        command["name"], isMemberCall, memberRemoval
                    ),
                    "params": params,
                    "param_count": len(params),
                    "last_result_is_array": lastResultIsArray,
                    "last_result": lastResult,
                    "results": results,
                    "result_count": len(results),
                    "typeid": typeID,
                    "interp": interp,
                }
            )
            typeID = typeID + 1

        write(json.dumps(json_data), file=self.outFile)
        OutputGenerator.endFile(self)

    def beginFeature(self, interface, emit):
        # Start processing in superclass
        OutputGenerator.beginFeature(self, interface, emit)
        # self.commands = []
        # self.structs = []

    def endFeature(self):
        "Actually write the interface to the output file."

        # self.features[self.featureName] = {
        #    "name": self.featureName,
        #    "commands": self.commands,
        #    "structs": self.structs,
        # }
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

        ignoredStructs = [
            "VkPipelineRobustnessCreateInfoEXT",
            "VkPipelineShaderStageModuleIdentifierCreateInfoEXT",
            "VkPhysicalDeviceFragmentShaderBarycentricPropertiesKHR",
            "VkPhysicalDevicePipelineRobustnessPropertiesEXT",
            "VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT",
            "VkPhysicalDeviceShaderModuleIdentifierPropertiesEXT",
            "VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR",
            "VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR",
            "VkPhysicalDevicePipelineRobustnessFeaturesEXT",
            "VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT",
            "VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD",
            "VkPhysicalDeviceImageCompressionControlFeaturesEXT",
            "VkPhysicalDevicePipelinePropertiesFeaturesEXT",
            "VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT",
            "VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT",
            "VkPhysicalDeviceImage2DViewOf3DFeaturesEXT",
            "VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE",
            "VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT",
            "VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT",
            "VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT",
            "VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT",
            "VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR",
            "VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR",
            "VkPhysicalDevicePipelineRobustnessFeaturesEXT",
            "VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT",
            "VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD",
            "VkPhysicalDeviceImageCompressionControlFeaturesEXT",
            "VkPhysicalDevicePipelinePropertiesFeaturesEXT",
            "VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT",
            "VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT",
            "VkPhysicalDeviceImage2DViewOf3DFeaturesEXT",
            "VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE",
            "VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT",
            "VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT",
            "VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT",
            "VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT",
            "VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR",
            "VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR",
            "VkPhysicalDevicePipelineRobustnessFeaturesEXT",
            "VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT",
            "VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD",
            "VkPhysicalDeviceImageCompressionControlFeaturesEXT",
            "VkPhysicalDevicePipelinePropertiesFeaturesEXT",
            "VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT",
            "VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT",
            "VkPhysicalDeviceImage2DViewOf3DFeaturesEXT",
            "VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE",
            "VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT",
            "VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT",
            "VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT",
            "VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT",
            "VkMultisampledRenderToSingleSampledInfoEXT",
            "VkImageCompressionPropertiesEXT",
            "VkImageCompressionControlEXT",
            "VkImageCompressionControlEXT",
            "VkImageCompressionControlEXT",
            "VkMultisampledRenderToSingleSampledInfoEXT",
            "VkRenderPassCreationControlEXT",
            "VkRenderPassSubpassFeedbackCreateInfoEXT",
            "VkPipelineRobustnessCreateInfoEXT",
            "VkGraphicsPipelineLibraryCreateInfoEXT",
            "VkPipelineRobustnessCreateInfoEXT",
            "VkPipelineRobustnessCreateInfoEXT",
            "VkSubpassResolvePerformanceQueryEXT",
            "VkRenderPassCreationControlEXT",
            "VkRenderPassCreationFeedbackCreateInfoEXT",
            "VkImageCompressionPropertiesEXT",
            "VkSubresourceLayout2EXT",
            "VkSubresourceLayout2EXT",
            "VkImageCompressionPropertiesEXT",
            # following are after adding other features
            "VkImageSubresource2EXT",
            "VkPipelinePropertiesIdentifierEXT",
            "VkDescriptorSetBindingReferenceVALVE",
            "VkDescriptorSetLayoutHostMappingInfoVALVE",
            "VkShaderModuleIdentifierEXT",
            "VkSubresourceLayout2EXT",
            "VkPipelineInfoEXT",
            "VkTraceRaysIndirectCommand2KHR",
            "VkTraceRaysIndirectCommandKHR",
            "VkRenderPassCreationFeedbackInfoEXT",
            "VkRenderPassSubpassFeedbackInfoEXT",
            "VkSubpassMergeStatusEXT",
        ]

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
                # if "VK_VERSION_1_0" == self.featureName:
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
            param_struct = False
            param_ref = 0
            param_optional = 0
            param_len = []
            param_is_array = False

            if member.text:
                if member.text == "const ":
                    param_const = True
                elif member.text == "struct ":
                    param_struct = True
                elif member.text == "const struct ":
                    param_const = True
                    param_struct = True
                else:
                    raise Exception("Unrecognised member text: " + member.text)

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
                    param_is_array = True
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
                    "struct": param_struct,
                    "ref": param_ref,
                    "opt": param_optional,
                    "is_len": False,
                    "is_array": param_is_array,
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

        commandIgnorList = [
            "vkGetImageSubresourceLayout2EXT",
            "vkGetPipelinePropertiesEXT",
            "vkGetDescriptorSetLayoutHostMappingInfoVALVE",
            "vkGetShaderModuleIdentifierEXT",
            "vkGetShaderModuleCreateInfoIdentifierEXT",
            # fix later...
            "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR",
            "vkAcquireWinrtDisplayNV",
            "vkGetWinrtDisplayNV",

            "vkCmdBuildAccelerationStructuresKHR",
            "vkCmdBuildAccelerationStructuresIndirectKHR",
            "vkBuildAccelerationStructuresKHR",
            "vkWriteAccelerationStructuresPropertiesKHR",
            "vkGetAccelerationStructureBuildSizesKHR",
            "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR",
            "vkGetDeviceImageMemoryRequirements",
            "vkGetPhysicalDeviceSurfaceCapabilities2KHR",
            "vkGetDeviceImageMemoryRequirementsKHR",
            "vkGetCalibratedTimestampsEXT"
        ]

        if name in commandIgnorList:
            return

        if "Destroy" in name:
            return

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
            param_struct = False
            param_ref = 0
            param_optional = 0
            param_len = []
            param_is_array = False
            param_is_carray = False

            if param.text:
                if param.text == "const ":
                    param_const = True
                elif param.text == "struct ":
                    param_struct = True
                elif param.text == "const struct ":
                    param_struct = True
                    param_const = True
                else:
                    raise Exception("Unrecognised param text: " + param.text)

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
                    param_is_array = True
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
                    if elem.tail:
                        if elem.tail.strip() == "[2]":
                            param_type = "std::array< " + self.calculateTypeName(False, 0, param_type) + ", 2 >"
                            param_is_carray = True
                        elif elem.tail.strip() == "[4]":
                            param_type = "std::array< " + self.calculateTypeName(False, 0, param_type) + ", 4 >"
                            param_is_carray = True
                        else:
                            raise Exception( "Unrecognised param elem tail" )
                else:
                    raise Exception("Unrecognised param element")

            command_parameters.append(
                {
                    "type": param_type,
                    "name": param_name,
                    "const": param_const,
                    "struct": param_struct,
                    "ref": param_ref,
                    "opt": param_optional,
                    "is_array": param_is_array,
                    "is_c_array": param_is_carray,
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
