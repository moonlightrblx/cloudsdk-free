#pragma once
#include <cstdint>
#include "../math/vector.h"

namespace UE4 {
    // Basic stuff for now, expand later
    struct FName {
        int32_t ComparisonIndex;
        int32_t Number;
    };

    template<class T>
    struct TArray {
        friend struct FString;

    public:
        inline int Num() const { return Count; };
        inline T& operator[](int i) { return Data[i]; };

    private:
        T* Data;
        int32_t Count;
        int32_t Max;
    };

    struct FString : private TArray<wchar_t> {
        inline const wchar_t* c_str() const { return Data; }
    };

    struct UObject {
        void** VTable;
        int32_t ObjectFlags;
        int32_t InternalIndex;
        UObject* ClassPrivate;
        FName NamePrivate;
        UObject* OuterPrivate;

        // wrapper for ease of use
        std::string GetName() {
            // simplified: implement actual name getting later with GNames
            return "UObject";
        }
    };
}
