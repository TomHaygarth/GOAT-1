cmake_minimum_required(VERSION 3.16)

# MacOs copy vulkan dependencies into 
# --------------------------------------------------------------------------------
macro(copy_vulkan_libs_to_target_bundle
    target_name
)
    if (BUILD_FOR_MAC)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${VULKAN_EXTERNALS_DIR}/macOS/lib/libvulkan.1.2.135.dylib $<TARGET_BUNDLE_CONTENT_DIR:${target_name}>/MacOS/libvulkan.1.2.135.dylib
            COMMAND ${CMAKE_COMMAND} -E create_symlink libvulkan.1.2.135.dylib $<TARGET_BUNDLE_CONTENT_DIR:${target_name}>/MacOS/libvulkan.1.dylib
            COMMAND ${CMAKE_COMMAND} -E copy ${VULKAN_EXTERNALS_DIR}/macOS/share/vulkan/icd.d/MoltenVK_icd.json $<TARGET_BUNDLE_CONTENT_DIR:${target_name}>/Resources/vulkan/icd.d/MoltenVK_icd.json
            COMMAND ${CMAKE_COMMAND} -E copy ${VULKAN_EXTERNALS_DIR}/macOS/lib/libMoltenVK.dylib $<TARGET_BUNDLE_CONTENT_DIR:${target_name}>/Frameworks/libMoltenVK.dylib
        )
    endif()
endmacro(copy_vulkan_libs_to_target_bundle)

# macro(set_project_version target major minor patch)
#     if (BUILD_FOR_MAC)
#     endif()
# endmacro(set_project_version)
