// Created by Brandon Davis

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

protected:

	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	/*
	 * Callbacks for the custom delegates on the MultiplayerSessionSubsystem
	 */

	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	UFUNCTION()
	void OnDestorySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
private:

	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();

	// The subsystem designed to handle all online session functionality
	class UMultiplayerSessionSubsystem* MultiplayerSessionSubsystem;

	int32 NumPublicConnections = 4;
	FString MatchType = TEXT("FreeForAll");
	FString PathToLobby = TEXT("");
};
