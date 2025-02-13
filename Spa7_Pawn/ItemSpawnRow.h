#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" // FTableRowBase 정의가 들어있는 헤더
#include "ItemSpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FItemSpawnRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    // 아이템 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemName;
    // 어떤 아이템 클래스를 스폰할지
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> ItemClass; 
    // >>하드 레퍼런스 : 클래스가 항상 메모리에 로드된 상태에서 접근
    // >> 모든 클래스를 메모리에 올리는것은 권장하는 형태는 아님.
    // 별도의 설정을 해줘야함.
    // 
    // TSoftClassPtr (소프트 레퍼런스): 클래스의 경로만 유지.
    // 이 아이템의 스폰 확률
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnChance;
};