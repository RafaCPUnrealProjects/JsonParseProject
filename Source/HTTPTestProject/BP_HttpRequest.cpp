// Fill out your copyright notice in the Description page of Project Settings.

#include "BP_HttpRequest.h"
#include <JsonObject.h>



// Sets default values
ABP_HttpRequest::ABP_HttpRequest(const class FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Http = &FHttpModule::Get();

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;
	mesh->bUseAsyncCooking = true;
}

// void ABP_HttpRequest::PostActorCreated()
// {
// 	Super::PostActorCreated();
// 	CreateTriangle();
// }

// void ABP_HttpRequest::PostLoad()
// {
// 	Super::PostActorCreated();
// 	CreateTriangle();
// }

void ABP_HttpRequest::CreateTriangle(TArray<FVector> vertices, TArray<int32> Triangles, TArray<FVector> normals, TArray<FVector2D> UV0, TArray<FProcMeshTangent> tangents, TArray<FLinearColor> vertexColors)
{
	// 	TArray<FVector> vertices;
	// 	vertices.Add(FVector(0, 0, 0));
	// 	vertices.Add(FVector(0, 100, 0));
	// 	vertices.Add(FVector(0, 0, 100));
	// 
	// 	TArray<int32> Triangles;
	// 	Triangles.Add(0);
	// 	Triangles.Add(1);
	// 	Triangles.Add(2);
	// 
	// 	TArray<FVector> normals;
	// 	normals.Add(FVector(1, 0, 0));
	// 	normals.Add(FVector(1, 0, 0));
	// 	normals.Add(FVector(1, 0, 0));
	// 
	// 	TArray<FVector2D> UV0;
	// 	UV0.Add(FVector2D(0, 0));
	// 	UV0.Add(FVector2D(10, 0));
	// 	UV0.Add(FVector2D(0, 10));
	// 
	// 
	// 	TArray<FProcMeshTangent> tangents;
	// 	tangents.Add(FProcMeshTangent(0, 1, 0));
	// 	tangents.Add(FProcMeshTangent(0, 1, 0));
	// 	tangents.Add(FProcMeshTangent(0, 1, 0));
	// 
	// 	TArray<FLinearColor> vertexColors;
	// 	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	// 	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	// 	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));

	mesh->CreateMeshSection_LinearColor(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);

	// Enable collision data
	mesh->ContainsPhysicsTriMeshData(true);
}

// Called when the game starts or when spawned
void ABP_HttpRequest::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));
	MyHttpCall();
	//CreateTriangle();
	Super::BeginPlay();

}

void ABP_HttpRequest::MyHttpCall()
{
	UE_LOG(LogTemp, Warning, TEXT("MyHttpCall"));
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ABP_HttpRequest::OnResponseReceived);
	//this is the url on which to process the request
	Request->SetURL("https://unreal-json-project.herokuapp.com/");
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}

void ABP_HttpRequest::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnResponseReceived %s"), *Response->GetContentAsString());
	//pointer to json serialized data
	TSharedPtr<FJsonObject> JsonObject;

	//reader pointer to read json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//deserialize the json data given Reader and the actual object to deserialize
	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("Deserialize ok"));

		ExtractVertices(JsonObject, vertices);

		ExtractTriangles(JsonObject, triangles);

		ExtractNormals(JsonObject, normals);

		ExtractUV0(JsonObject, UV0);

		ExtractTangents(JsonObject, tangents);

		ExtractVertexColors(JsonObject, vertexColors);

		if (vertices.Num() > 0 &&
			triangles.Num() > 0 &&
			normals.Num() > 0 &&
			UV0.Num() > 0 &&
			tangents.Num() > 0 &&
			vertexColors.Num() > 0 )
		{
			CreateTriangle(vertices, triangles, normals, UV0, tangents, vertexColors);
		}
	}
}

void ABP_HttpRequest::ExtractVertexColors(TSharedPtr<FJsonObject> JsonObject, TArray<FLinearColor> &vertexColors)
{
	//VERTEXCOLORS
	const TArray<TSharedPtr<FJsonValue>> *vertColArrayPtr;

	if (JsonObject->TryGetArrayField(TEXT("vertexColors"), vertColArrayPtr))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryGetArrayField vertexColors ok"));

		TArray<TArray<TSharedPtr<FJsonValue>>> arr;

		for (size_t i = 0; i < vertColArrayPtr->Num(); i++)
		{
			auto item = (*vertColArrayPtr)[i]->AsArray();
			arr.Add(item);
		}

		for (size_t i = 0; i < arr.Num(); i++)
		{
			vertexColors.Add(FLinearColor(
				(float)arr[i][0]->AsNumber(),
				(float)arr[i][1]->AsNumber(),
				(float)arr[i][2]->AsNumber(),
				(float)arr[i][3]->AsNumber()
			));
		}
	}
}

void ABP_HttpRequest::ExtractTangents(TSharedPtr<FJsonObject> JsonObject, TArray<FProcMeshTangent> &tangents)
{
	//TANGENTS
	const TArray<TSharedPtr<FJsonValue>> *tangArrayPtr;

	if (JsonObject->TryGetArrayField(TEXT("tangents"), tangArrayPtr))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryGetArrayField tangents ok"));

		TArray<TArray<TSharedPtr<FJsonValue>>> arr;

		for (size_t i = 0; i < tangArrayPtr->Num(); i++)
		{
			auto item = (*tangArrayPtr)[i]->AsArray();
			arr.Add(item);
		}

		for (size_t i = 0; i < arr.Num(); i++)
		{
			tangents.Add(FProcMeshTangent(
				(float)arr[i][0]->AsNumber(),
				(float)arr[i][1]->AsNumber(),
				(float)arr[i][2]->AsNumber()
			));
		}
	}
}

void ABP_HttpRequest::ExtractUV0(TSharedPtr<FJsonObject> JsonObject, TArray<FVector2D> &UV0)
{
	//UV0
	const TArray<TSharedPtr<FJsonValue>> *uv0ArrayPtr;

	if (JsonObject->TryGetArrayField(TEXT("UV0"), uv0ArrayPtr))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryGetArrayField UV0 ok"));

		TArray<TArray<TSharedPtr<FJsonValue>>> arr;

		for (size_t i = 0; i < uv0ArrayPtr->Num(); i++)
		{
			auto item = (*uv0ArrayPtr)[i]->AsArray();
			arr.Add(item);
		}

		for (size_t i = 0; i < arr.Num(); i++)
		{
			UV0.Add(FVector2D(
				(float)arr[i][0]->AsNumber(),
				(float)arr[i][1]->AsNumber()
			));

			UE_LOG(LogTemp, Warning, TEXT("UV0 %i %f %f"), i, UV0[i][0], UV0[i][1]);
		}
	}
}

void ABP_HttpRequest::ExtractNormals(TSharedPtr<FJsonObject> JsonObject, TArray<FVector> &normals)
{
	//NORMALS
	const TArray<TSharedPtr<FJsonValue>> *normArrayPtr;

	if (JsonObject->TryGetArrayField(TEXT("normals"), normArrayPtr))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryGetArrayField normals ok"));

		TArray<TArray<TSharedPtr<FJsonValue>>> arr;

		for (size_t i = 0; i < normArrayPtr->Num(); i++)
		{
			auto item = (*normArrayPtr)[i]->AsArray();
			arr.Add(item);
		}

		for (size_t i = 0; i < arr.Num(); i++)
		{
			normals.Add(FVector(
				(float)arr[i][0]->AsNumber(),
				(float)arr[i][1]->AsNumber(),
				(float)arr[i][2]->AsNumber()
			));

			UE_LOG(LogTemp, Warning, TEXT("normals %i %f %f %f"), i, normals[i][0], normals[i][1], normals[i][2]);
		}
	}
}

void ABP_HttpRequest::ExtractTriangles(TSharedPtr<FJsonObject> JsonObject, TArray<int32> &triangles)
{
	//TRIANGLES
	const TArray<TSharedPtr<FJsonValue>> *triArrayPtr;

	if (JsonObject->TryGetArrayField(TEXT("triangles"), triArrayPtr))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryGetArrayField triangles ok"));

		TArray<TArray<TSharedPtr<FJsonValue>>> arr;

		for (size_t i = 0; i < triArrayPtr->Num(); i++)
		{
			auto item = (*triArrayPtr)[i]->AsNumber();
			triangles.Add((int32)item);
		}
	}
}

void ABP_HttpRequest::ExtractVertices(TSharedPtr<FJsonObject> JsonObject, TArray<FVector> &vertices)
{
	//VERTICES
	const TArray<TSharedPtr<FJsonValue>> *vertArrayPtr;

	if (JsonObject->TryGetArrayField(TEXT("vertices"), vertArrayPtr))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryGetArrayField vertices ok"));

		TArray<TArray<TSharedPtr<FJsonValue>>> arr;

		for (size_t i = 0; i < vertArrayPtr->Num(); i++)
		{
			auto item = (*vertArrayPtr)[i]->AsArray();
			arr.Add(item);
		}

		for (size_t i = 0; i < arr.Num(); i++)
		{
			vertices.Add(FVector(
				(float)arr[i][0]->AsNumber(),
				(float)arr[i][1]->AsNumber(),
				(float)arr[i][2]->AsNumber()
			));

			UE_LOG(LogTemp, Warning, TEXT("vertices %i %f %f %f"), i, vertices[i][0], vertices[i][1], vertices[i][2]);
		}
	}
}

// Called every frame
void ABP_HttpRequest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

