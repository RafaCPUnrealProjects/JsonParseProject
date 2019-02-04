// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "ProceduralMeshComponent.h"
#include "BP_HttpRequest.generated.h"

UCLASS()
class HTTPTESTPROJECT_API ABP_HttpRequest : public AActor
{
	GENERATED_BODY()
	
public:	

	FHttpModule* Http;

	UFUNCTION()
	void MyHttpCall();

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void ExtractVertexColors(TSharedPtr<FJsonObject> JsonObject, TArray<FLinearColor> &vertexColors);

	void ExtractTangents(TSharedPtr<FJsonObject> JsonObject, TArray<FProcMeshTangent> &tangents);

	void ExtractUV0(TSharedPtr<FJsonObject> JsonObject, TArray<FVector2D> &UV0);

	void ExtractNormals(TSharedPtr<FJsonObject> JsonObject, TArray<FVector> &normals);

	void ExtractTriangles(TSharedPtr<FJsonObject> JsonObject, TArray<int32> &triangles);

	void ExtractVertices(TSharedPtr<FJsonObject> JsonObject, TArray<FVector> &vertices);

	// Sets default values for this actor's properties
	ABP_HttpRequest(const class FObjectInitializer& ObjectInitializer);
	
protected:
	void CreateTriangle(TArray<FVector> vertices, TArray<int32> Triangles, TArray<FVector> normals, TArray<FVector2D> UV0, TArray<FProcMeshTangent> tangents, TArray<FLinearColor> vertexColors);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* mesh;
	
};
