#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "SBPawnCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
    ExplosionDelay = 0.3f;
    ExplosionRadius = 300.0f;
    ExplosionDamage = 30.0f;
    ItemType = "Mine";
    bHasExploded = false;

    ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
    ExplosionCollision->InitSphereRadius(ExplosionRadius);
    ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
    if (bHasExploded) return;
    Super::ActivateItem(Activator);

    GetWorld()->GetTimerManager().SetTimer(
        ExplosionTimerHandle,
        this,
        &AMineItem::Explode,
        ExplosionDelay,
        false
    );

    bHasExploded = true;
}

void AMineItem::Explode()
{
    UParticleSystemComponent* Particle = nullptr;

    if (ExplosionParticle)
    {
        Particle = UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ExplosionParticle,
            GetActorLocation(),
            GetActorRotation(),
            false
        );
    }

    TArray<AActor*> OverlappingActors;
    ExplosionCollision->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->ActorHasTag("Player"))
        {
            if (ASBPawnCharacter* Character = Cast<ASBPawnCharacter>(Actor)) {
                // 데미지를 발생시켜 Actor->TakeDamage()가 실행되도록 함
                UGameplayStatics::ApplyDamage(
                    Actor,                      // 데미지를 받을 액터
                    ExplosionDamage,            // 데미지 양
                    nullptr,                    // 데미지를 유발한 주체 (지뢰를 설치한 캐릭터가 없으므로 nullptr)
                    this,                       // 데미지를 유발한 오브젝트(지뢰)
                    UDamageType::StaticClass()  // 기본 데미지 유형
                );

            }
        }
    }

    // 지뢰 제거
    DestroyItem();

    if (Particle)
    {
        FTimerHandle DestroyParticleTimerHandle;
        TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

        GetWorld()->GetTimerManager().SetTimer(
            DestroyParticleTimerHandle,
            [WeakParticle]()
            {
                if (WeakParticle.IsValid())
                {
                    WeakParticle->DestroyComponent();
                }
            },
            2.0f,
            false
        );
    }
}