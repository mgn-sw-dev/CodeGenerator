# ToDo: Qt needed?
find_package(Qt6 REQUIRED COMPONENTS Core)
qt_add_library(Extern STATIC)
target_link_libraries(Extern PUBLIC
        Qt6::Core
)
target_include_directories(Extern PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_sources(Extern PRIVATE
        Inja/inja.hpp
        Nlohmann/json.hpp
        Nlohmann/json_fwd.hpp
        ValiJson/Adapters/NlohmannJsonAdapter.hpp
        ValiJson/Adapters/QtJsonAdapter.hpp
        ValiJson/Adapters/StdStringAdapter.hpp
        ValiJson/Constraints/BasicConstraint.hpp
        ValiJson/Constraints/ConcreteConstraints.hpp
        ValiJson/Constraints/Constraint.hpp
        ValiJson/Constraints/ConstraintVisitor.hpp
        ValiJson/Internal/Adapter.hpp
        ValiJson/Internal/BasicAdapter.hpp
        ValiJson/Internal/CustomAllocator.hpp
        ValiJson/Internal/Debug.hpp
        ValiJson/Internal/FrozenValue.hpp
        ValiJson/Internal/JsonPointer.hpp
        ValiJson/Internal/JsonReference.hpp
        ValiJson/Internal/Regex.hpp
        ValiJson/Internal/Uri.hpp
        ValiJson/Utils/FileUtils.hpp
        ValiJson/Utils/NlohmannJsonUtils.hpp
        ValiJson/Utils/QtJsonUtils.hpp
        ValiJson/Utils/Utf8Utils.hpp
        ValiJson/ConstraintBuilder.hpp
        ValiJson/Exceptions.hpp
        ValiJson/Schema.hpp
        ValiJson/SchemaCache.hpp
        ValiJson/SchemaParser.hpp
        ValiJson/Subschema.hpp
        ValiJson/ValidationResults.hpp
        ValiJson/ValidationVisitor.hpp
        ValiJson/Validator.hpp
)