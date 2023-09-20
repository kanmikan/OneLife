#include "SettingsPage.h"


#include "minorGems/game/game.h"
#include "minorGems/game/Font.h"

#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/stringUtils.h"

#include "minorGems/game/game.h"
#include "minorGems/system/Time.h"

#include "musicPlayer.h"
#include "soundBank.h"
#include "objectBank.h"
#include "buttonStyle.h"

#ifdef USE_DISCORD
#include "DiscordController.h"
#endif // USE_DISCORD

#include "DropdownList.h"

extern Font * mainFont;

extern float musicLoudness;

extern bool showingInGameSettings;

// defined in LivingLifePage.cpp
extern bool ShowUseOnObjectHoverSettingToggle;
extern bool isShowUseOnObjectHoverKeybindEnabled;

#ifdef USE_DISCORD
// extern from DiscordController.h
DiscordController *discordControllerInstance; 
#include <ctime>
time_t last_discord_setting_change = 0;
#endif // USE_DISCORD

SettingsPage::SettingsPage()
        : mBackground( "background.tga", 0.75f ),
          
          // Left Pane
          mRestartButton( mainFont, 360, -272, translate( "restartButton" ) ),
          
          mGameplayButton( mainFont, -452.5, 288, translate("gameplayButton") ),
          mControlButton( mainFont, -452.5, 192, translate("controlButton") ),
          mScreenButton( mainFont, -452.5, 96, translate("screenButton") ),
          mSoundButton( mainFont, -452.5, 0, translate("soundButton") ),
#ifdef USE_DISCORD
          mDiscordButton( mainFont, -452.5, -96, translate("discordButton") ),
#endif // USE_DISCORD
          mAdvancedButton( mainFont, -452.5, -192, translate("advancedButton") ),

          mBackButton( mainFont, -452.5, -288, translate( "backButton" ) ),
          
          mEditAccountButton( mainFont, -463, 129, translate( "editAccount" ) ),

          // Gameplay
		  mEnableFOVBox( 561, 128, 4 ),
		  mEnableCenterCameraBox( 561, 52, 4 ),
          mEnableNudeBox( -335, 148, 4 ),
          
          mUseCustomServerBox( -168, -148, 4 ),
          mCustomServerAddressField( mainFont, 306, -150, 14, false, 
                                     "",
                                     NULL,
                                     // forbid spaces
                                     " " ),
          mCustomServerPortField( mainFont, 84, -208, 4, false, 
                                  "",
                                  "0123456789", NULL ),
          mCopyButton( mainFont, 381, -216, translate( "copy" ) ),
          mPasteButton( mainFont, 518, -216, translate( "paste" ) ),
          
          // Control
		  mEnableKActionsBox( 561, 90, 4 ),
          mCursorScaleSlider( mainFont, 297, 155, 4, 200, 30,
                                       1.0, 10.0, 
                                       translate( "scale" ) ),
          
          // Screen
          mRedetectButton( mainFont, 153, 249, translate( "redetectButton" ) ),
          mFullscreenBox( 0, 128, 4 ),
          mBorderlessBox( 0, 168, 4 ),
          mTrippingEffectDisabledBox( 0, 168, 4 ),
          
          // Sound
          mMusicLoudnessSlider( mainFont, 0, 40, 4, 200, 30,
                                0.0, 1.0, 
                                translate( "musicLoudness" ) ),
          mSoundEffectsLoudnessSlider( mainFont, 0, -48, 4, 200, 30,
                                       0.0, 1.0, 
                                       translate( "soundLoudness" ) )
#ifdef USE_DISCORD
        , mEnableDiscordRichPresence(0, 168, 4),
          mEnableDiscordRichPresenceStatus(0, 128, 4), 
          mEnableDiscordShowAgeInStatus(0, 88, 4), 
          mEnableDiscordRichPresenceDetails(0, 48, 4),
          mDiscordHideFirstNameInDetails(0, 8, 4) 
#endif // USE_DISCORD
        , mEnableAdvancedShowUseOnObjectHoverKeybind(0, 168, 4) {
                            

    
    // Adding components below in reverse order so cursor tip is drawn on top
    
    addComponent( &mBackground );
    
    // Advanced
    addComponent(&mEnableAdvancedShowUseOnObjectHoverKeybind);
    mEnableAdvancedShowUseOnObjectHoverKeybind.addActionListener(this);
    
#ifdef USE_DISCORD
    // Discord
    addComponent(&mDiscordHideFirstNameInDetails);
    mDiscordHideFirstNameInDetails.addActionListener(this);
    addComponent(&mEnableDiscordRichPresenceDetails);
    mEnableDiscordRichPresenceDetails.addActionListener(this);
    addComponent(&mEnableDiscordShowAgeInStatus);
    mEnableDiscordShowAgeInStatus.addActionListener(this);
    addComponent(&mEnableDiscordRichPresenceStatus);
    mEnableDiscordRichPresenceStatus.addActionListener(this);
    addComponent(&mEnableDiscordRichPresence);
    mEnableDiscordRichPresence.addActionListener(this);
#endif // USE_DISCORD
    
    // Sound
    addComponent( &mSoundEffectsLoudnessSlider );
    mSoundEffectsLoudnessSlider.addActionListener( this );
    addComponent( &mMusicLoudnessSlider );
    mMusicLoudnessSlider.addActionListener( this );
    
    // Screen
    addComponent( &mTrippingEffectDisabledBox );
    mTrippingEffectDisabledBox.addActionListener( this );
    addComponent( &mBorderlessBox );
    mBorderlessBox.addActionListener( this );
    addComponent( &mFullscreenBox );
    mFullscreenBox.addActionListener( this );
    setButtonStyle( &mRedetectButton );
    addComponent( &mRedetectButton );
    mRedetectButton.addActionListener( this );
    
    // Control
    addComponent( &mCursorScaleSlider );
    mCursorScaleSlider.addActionListener( this );
    mCursorScaleSlider.toggleField( false );
    
    const char *choiceList[3] = { translate( "system" ),
                                  translate( "drawn" ),
                                  translate( "both" ) };
    
    mCursorModeSet = 
        new RadioButtonSet( mainFont, 561, 275,
                            3, choiceList,
                            false, 4 );
    addComponent( mCursorModeSet );
    mCursorModeSet->addActionListener( this );
    
	addComponent( &mEnableKActionsBox );
    mEnableKActionsBox.addActionListener( this );
    
    // Gameplay
    setButtonStyle( &mPasteButton );
    addComponent( &mPasteButton );
    mPasteButton.addActionListener( this );
    setButtonStyle( &mCopyButton );
    addComponent( &mCopyButton );
    mCopyButton.addActionListener( this );
    addComponent( &mCustomServerPortField );
    mCustomServerPortField.setWidth( 360 );
    mCustomServerPortField.setHigh( 40 );
    addComponent( &mCustomServerAddressField );    
    mCustomServerAddressField.setWidth( 360 );
    mCustomServerAddressField.setHigh( 40 );
    addComponent( &mUseCustomServerBox );
    mUseCustomServerBox.addActionListener( this );
    
    addComponent( &mEnableNudeBox );
    mEnableNudeBox.addActionListener( this );
	addComponent( &mEnableCenterCameraBox );
    mEnableCenterCameraBox.addActionListener( this );
	addComponent( &mEnableFOVBox );
    mEnableFOVBox.addActionListener( this );
    
    // Left pane
    setButtonStyle(&mAdvancedButton);
    mAdvancedButton.setSize(175, 60);
    addComponent(&mAdvancedButton);
    mAdvancedButton.addActionListener(this);
    
#ifdef USE_DISCORD
    setButtonStyle(&mDiscordButton);
    mDiscordButton.setSize(175, 60);
    addComponent(&mDiscordButton);
    mDiscordButton.addActionListener(this);
#endif // USE_DISCORD
    
    setButtonStyle( &mBackButton );
    mBackButton.setSize( 175, 60 );
    addComponent( &mBackButton );
    mBackButton.addActionListener( this );
    
    setButtonStyle( &mSoundButton );
    mSoundButton.setSize( 175, 60 );
    addComponent( &mSoundButton );
    mSoundButton.addActionListener( this );
    
    setButtonStyle( &mScreenButton );
    mScreenButton.setSize( 175, 60 );
    addComponent( &mScreenButton );
    mScreenButton.addActionListener( this );
    
    setButtonStyle( &mControlButton );
    mControlButton.setSize( 175, 60 );
    addComponent( &mControlButton );
    mControlButton.addActionListener( this );
    
    setButtonStyle( &mGameplayButton );
    mGameplayButton.setSize( 175, 60 );
    addComponent( &mGameplayButton );
    mGameplayButton.addActionListener( this );



    setButtonStyle( &mRestartButton );
    mRestartButton.setSize( 175, 60 );
    addComponent( &mRestartButton );
    mRestartButton.addActionListener( this );
    mRestartButton.setVisible( false );


    // Not in use
    setButtonStyle( &mEditAccountButton );
    // addComponent( &mEditAccountButton );
    mEditAccountButton.addActionListener( this );
    
    mRestartButton.setCursorTip( translate("restartButtonTip") );
    
    mGameplayButton.setCursorTip( translate("gameplayButtonTip") );
    mControlButton.setCursorTip( translate("controlButtonTip") );
    mScreenButton.setCursorTip( translate("screenButtonTip") );
    mSoundButton.setCursorTip( translate("soundButtonTip") );
#ifdef USE_DISCORD
    mDiscordButton.setCursorTip( translate("discordButtonTip") );
#endif // USE_DISCORD
    mAdvancedButton.setCursorTip( translate("advancedButtonTip") );

    mBackButton.setCursorTip( translate("backButtonTip") );
    
    mEnableFOVBox.setCursorTip( translate("enableFovTip") );
    mEnableCenterCameraBox.setCursorTip( translate("enableCenterCameraTip") );
    mEnableNudeBox.setCursorTip( translate("enableNudeTip") );
    
    mUseCustomServerBox.setCursorTip( translate("customServerTip") );
    mCustomServerAddressField.setCursorTip( translate("customServerAddressTip") );
    mCustomServerPortField.setCursorTip( translate("customServerPortTip") );
    
    mEnableKActionsBox.setCursorTip( translate("enableWasdTip") );
    mCursorModeSet->setCursorTip( translate("cursorModeTip") );
    
    mRedetectButton.setCursorTip( translate("redetectButtonTip") );
    mFullscreenBox.setCursorTip( translate("fullscreenTip") );
    mBorderlessBox.setCursorTip( translate("borderlessTip") );
    mTrippingEffectDisabledBox.setCursorTip( translate("trippingEffectTip") );

#ifdef USE_DISCORD
    // Discord
    mEnableDiscordRichPresence.setCursorTip( translate("enableDiscordRichPreenceTip") );
    mEnableDiscordRichPresenceStatus.setCursorTip( translate("enableDiscordRichPreenceStatusTip") );
    mEnableDiscordShowAgeInStatus.setCursorTip( translate("enableDiscordRichPreenceStatusAgeTip") );
    mEnableDiscordRichPresenceDetails.setCursorTip( translate("enableDiscordRichPreenceDetailsTip") );
    mDiscordHideFirstNameInDetails.setCursorTip( translate("discordHideFirstName") );
#endif // USE_DISCORD

    mEnableAdvancedShowUseOnObjectHoverKeybind.setCursorTip(
      translate("objectRemainingUseTip") );
    
    mOldFullscreenSetting = 
        SettingsManager::getIntSetting( "fullscreen", 1 );
    
    mTestSound = blankSoundUsage;

    mMusicStartTime = 0;

    mFullscreenBox.setToggled( mOldFullscreenSetting );

    mBorderlessBox.setVisible( mOldFullscreenSetting );


    mOldBorderlessSetting = 
        SettingsManager::getIntSetting( "borderless", 0 );

    mBorderlessBox.setToggled( mOldBorderlessSetting );
    
    mTrippingEffectDisabledSetting =
        SettingsManager::getIntSetting( "trippingEffectDisabled", 1 );

    mTrippingEffectDisabledBox.setToggled( mTrippingEffectDisabledSetting );

    mEnableNudeSetting =
        SettingsManager::getIntSetting( "nudeEnabled", 1 );

    mEnableNudeBox.setToggled( mEnableNudeSetting );
	
	mEnableFOVSetting =
        SettingsManager::getIntSetting( "fovEnabled", 0 );
	
	mEnableFOVBox.setToggled( mEnableFOVSetting );
    
	mEnableKActionsSetting =
        SettingsManager::getIntSetting( "keyboardActions", 0 );
	
	mEnableKActionsBox.setToggled( mEnableKActionsSetting );
        
	mEnableCenterCameraSetting =
        SettingsManager::getIntSetting( "centerCamera", 0 );
	
	mEnableCenterCameraBox.setToggled( mEnableCenterCameraSetting );

#ifdef USE_DISCORD
    mDiscordRichPresenceSetting =
        SettingsManager::getIntSetting("discordRichPresence", 1);

    mEnableDiscordRichPresence.setToggled(mDiscordRichPresenceSetting);

    mDiscordRichPresenceStatusSetting =
        SettingsManager::getIntSetting("discordRichPresenceStatus", 1);

    mDiscordShowAgeInStatusSetting =
        SettingsManager::getIntSetting("discordRichPresenceShowAge", 1);

    mEnableDiscordShowAgeInStatus.setToggled(mDiscordShowAgeInStatusSetting);

    mEnableDiscordRichPresenceStatus.setToggled(mDiscordRichPresenceStatusSetting);

    mDiscordRichPresenceDetailsSetting =
        SettingsManager::getIntSetting("discordRichPresenceDetails", 0);

    mEnableDiscordRichPresenceDetails.setToggled(mDiscordRichPresenceDetailsSetting);

    mDiscordHideFirstNameInDetailsSetting =
        SettingsManager::getIntSetting("discordRichPresenceHideFirstName", 1);

    mDiscordHideFirstNameInDetails.setToggled(mDiscordHideFirstNameInDetailsSetting);
#endif // USE_DISCORD

    mAdvancedShowUseOnObjectHoverKeybindSetting = 
        SettingsManager::getIntSetting("showUseOnObjectHoverKeybind", 0);

    mEnableAdvancedShowUseOnObjectHoverKeybind.setToggled(
        mAdvancedShowUseOnObjectHoverKeybindSetting);

    mPage = 0;
    




    }



SettingsPage::~SettingsPage() {
    clearSoundUsage( &mTestSound );

    delete mCursorModeSet;
    }




void SettingsPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mBackButton ) {
        
        int useCustomServer = 0;
        if( mUseCustomServerBox.getToggled() ) {
            useCustomServer = 1;
            }
        
        SettingsManager::setSetting( "useCustomServer", useCustomServer );
        char *address = mCustomServerAddressField.getText();
        
        SettingsManager::setSetting( "customServerAddress", address );
        delete [] address;
        
        SettingsManager::setSetting( "customServerPort",
                                     mCustomServerPortField.getInt() );
        
        setSignal( "back" );
        setMusicLoudness( 0 );
        }
    else if( inTarget == &mEditAccountButton ) {
        setSignal( "editAccount" );
        setMusicLoudness( 0 );
        }
    else if( inTarget == &mFullscreenBox ) {
        int newSetting = mFullscreenBox.getToggled();
        
        SettingsManager::setSetting( "fullscreen", newSetting );
        }
    else if( inTarget == &mBorderlessBox ) {
        int newSetting = mBorderlessBox.getToggled();
        
        SettingsManager::setSetting( "borderless", newSetting );
        }
    else if( inTarget == &mTrippingEffectDisabledBox ) {
        int newSetting = mTrippingEffectDisabledBox.getToggled();
        
        SettingsManager::setSetting( "trippingEffectDisabled", newSetting );
        
        trippingEffectDisabled = newSetting;
        }
	else if( inTarget == &mEnableNudeBox ) {
        int newSetting = mEnableNudeBox.getToggled();
        
        SettingsManager::setSetting( "nudeEnabled", newSetting );
        
        //extern variable in objectBank removes the need to restart the game
        //we just need to set the new value to the variable and the game will
        //start ignoring the nudity sprites every times it draws a new object
        //mRestartButton.setVisible( mEnableNudeSetting != newSetting );
        NudeToggle = newSetting;
        }
	else if( inTarget == &mUseCustomServerBox ) {
        mCustomServerAddressField.setVisible( mPage == 0 && mUseCustomServerBox.getToggled() );
        mCustomServerPortField.setVisible( mPage == 0 && mUseCustomServerBox.getToggled() );
        }
	else if( inTarget == &mEnableFOVBox ) {
        int newSetting = mEnableFOVBox.getToggled();
        
        SettingsManager::setSetting( "fovEnabled", newSetting );
        }
	else if( inTarget == &mEnableKActionsBox ) {
        int newSetting = mEnableKActionsBox.getToggled();
        
        SettingsManager::setSetting( "keyboardActions", newSetting );
        }
	else if( inTarget == &mEnableCenterCameraBox ) {
        int newSetting = mEnableCenterCameraBox.getToggled();
        
        SettingsManager::setSetting( "centerCamera", newSetting );
        }
    else if( inTarget == &mRestartButton ||
             inTarget == &mRedetectButton ) {
        // always re-measure frame rate after relaunch
        SettingsManager::setSetting( "targetFrameRate", -1 );
        SettingsManager::setSetting( "countingOnVsync", -1 );
        
        char relaunched = relaunchGame();
        
        if( !relaunched ) {
            printf( "Relaunch failed\n" );
            setSignal( "relaunchFailed" );
            }
        else {
            printf( "Relaunched... but did not exit?\n" );
            setSignal( "relaunchFailed" );
            }
        }
    else if( inTarget == &mSoundEffectsLoudnessSlider ) {
        setMusicLoudness( 0 );
        mMusicStartTime = 0;
        
        if( ! mSoundEffectsLoudnessSlider.isPointerDown() ) {
            
            setSoundEffectsLoudness( 
                mSoundEffectsLoudnessSlider.getValue() );
        
            if( mTestSound.numSubSounds > 0 ) {
                doublePair pos = { 0, 0 };
                
                playSound( mTestSound, pos );
                }
            }
        }    
    else if( inTarget == &mMusicLoudnessSlider ) {
            

        if( ! mSoundEffectsLoudnessSlider.isPointerDown() ) {
            musicLoudness = mMusicLoudnessSlider.getValue();
            SettingsManager::setSetting( "musicLoudness", musicLoudness );
            }
            
        
        if( Time::getCurrentTime() - mMusicStartTime > 25 ) {

            instantStopMusic();
            

            restartMusic( 9.0, 1.0/60.0, true );
            
            setMusicLoudness( mMusicLoudnessSlider.getValue(), true );


            mMusicStartTime = Time::getCurrentTime();
            }
        else {
            setMusicLoudness( mMusicLoudnessSlider.getValue(), true );
            }
        }
    else if( inTarget == &mCopyButton ) {
        char *address = mCustomServerAddressField.getText();
        
        char *fullAddress = autoSprintf( "%s:%d", address,
                                         mCustomServerPortField.getInt() );
        delete [] address;
        
        setClipboardText( fullAddress );
        
        delete [] fullAddress;
        }
    else if( inTarget == &mPasteButton ) {
        char *text = getClipboardText();

        char *trimmed = trimWhitespace( text );
        
        delete [] text;
        

        char setWithPort = false;
        
        if( strstr( trimmed, ":" ) != NULL ) {
            char addressBuff[100];
            int port = 0;
            
            int numRead = sscanf( trimmed, "%99[^:]:%d", addressBuff, &port );
            
            if( numRead == 2 ) {
                setWithPort = true;
                
                char *trimmedAddr = trimWhitespace( addressBuff );
                
                // terminate at first space, if any
                char *spacePos = strstr( trimmedAddr, " " );
                if( spacePos != NULL ) {
                    spacePos[0] = '\0';
                    }

                mCustomServerAddressField.setText( trimmedAddr );

                delete [] trimmedAddr;
                
                mCustomServerPortField.setInt( port );
                }
            }
        
        if( ! setWithPort ) {
            // treat the whole thing as an address
            
            // terminate at first space, if any
            char *spacePos = strstr( trimmed, " " );
            
            if( spacePos != NULL ) {
                spacePos[0] = '\0';
                }
            mCustomServerAddressField.setText( trimmed );
            }
        delete [] trimmed;
        }
    else if( inTarget == mCursorModeSet ) {
        setCursorMode( mCursorModeSet->getSelectedItem() );
        
        // int mode = getCursorMode();
        
        // mCursorScaleSlider.setVisible( mode > 0 );
        
        mCursorScaleSlider.setValue( getEmulatedCursorScale() );
        }
    else if( inTarget == &mCursorScaleSlider ) {
        setEmulatedCursorScale( mCursorScaleSlider.getValue() );
        }
    else if( inTarget == &mGameplayButton ) {
        mPage = 0;
        }
    else if( inTarget == &mControlButton ) {
        mPage = 1;
        }
    else if( inTarget == &mScreenButton ) {
        mPage = 2;
        }
    else if( inTarget == &mSoundButton ) {
        mPage = 3;
        }

#ifdef USE_DISCORD
    else if( inTarget == &mDiscordButton ) {
        mPage = 4;
        }
    else if( inTarget == &mEnableDiscordRichPresence ) {
        int newSetting = mEnableDiscordRichPresence.getToggled();
        last_discord_setting_change = time(0);
        mDiscordRichPresenceSetting = newSetting;
        SettingsManager::setSetting("discordRichPresence", newSetting);
        if(discordControllerInstance != NULL) {
            discordControllerInstance->updateDisplayGame((char)newSetting);
            }
        }
    else if( inTarget == &mEnableDiscordRichPresenceStatus ) {
        last_discord_setting_change = time(0);
        int newSetting = mEnableDiscordRichPresenceStatus.getToggled();
        mDiscordRichPresenceStatusSetting = newSetting;
        SettingsManager::setSetting("discordRichPresenceStatus", newSetting);
        if(discordControllerInstance != NULL) {
            discordControllerInstance->updateDisplayStatus((char)newSetting);
            }
        }
    else if( inTarget == &mEnableDiscordShowAgeInStatus ) {
        last_discord_setting_change = time(0);
        int newSetting = mEnableDiscordShowAgeInStatus.getToggled();
        mDiscordShowAgeInStatusSetting = newSetting;
        SettingsManager::setSetting("discordRichPresenceShowAge", newSetting);
        if(discordControllerInstance != NULL) {
            discordControllerInstance->updateDisplayAge((char)newSetting);
            }
        }
    else if( inTarget == &mEnableDiscordRichPresenceDetails ) {
        last_discord_setting_change = time(0);
        int newSetting = mEnableDiscordRichPresenceDetails.getToggled();
        mDiscordRichPresenceDetailsSetting = newSetting;
        SettingsManager::setSetting("discordRichPresenceDetails", newSetting);
        if(discordControllerInstance != NULL) {
            discordControllerInstance->updateDisplayDetails((char)newSetting);
            }
        }
    else if( inTarget == &mDiscordHideFirstNameInDetails ) {
        last_discord_setting_change = time(0);
        int newSetting = mDiscordHideFirstNameInDetails.getToggled();
        mDiscordHideFirstNameInDetailsSetting = newSetting;
        SettingsManager::setSetting("discordRichPresenceHideFirstName", newSetting);
        if(discordControllerInstance != NULL) {
            discordControllerInstance->updateDisplayFirstName((char)!newSetting);
            }
        }        
#endif // USE_DISCORD

    else if ( inTarget == &mAdvancedButton ) {
        mPage = 5;
    }
    else if ( inTarget == &mEnableAdvancedShowUseOnObjectHoverKeybind ) {
        int newSetting = mEnableAdvancedShowUseOnObjectHoverKeybind.getToggled();
        mAdvancedShowUseOnObjectHoverKeybindSetting = newSetting;
        SettingsManager::setSetting("showUseOnObjectHoverKeybind",
                                    newSetting);
        ShowUseOnObjectHoverSettingToggle = (bool)newSetting;
        if( ShowUseOnObjectHoverSettingToggle ) isShowUseOnObjectHoverKeybindEnabled = true;
    }

    checkRestartRequired();
    updatePage();
    }


extern int targetFramesPerSecond;


void SettingsPage::draw( doublePair inViewCenter, 
                         double inViewSize ) {
    setDrawColor( 1, 1, 1, 1 );
    
    if( mFullscreenBox.isVisible() ) {
        doublePair pos = mFullscreenBox.getPosition();
        
        pos.x -= 30;
        pos.y -= 2;
        
        mainFont->drawString( translate( "fullscreen" ), pos, alignRight );


        if( mBorderlessBox.isVisible() ) {
            pos = mBorderlessBox.getPosition();
        
            pos.x -= 30;
            pos.y -= 2;
            
            mainFont->drawString( translate( "borderless" ), pos, alignRight );
            }


        if( mTrippingEffectDisabledBox.isVisible() ) {
            pos = mTrippingEffectDisabledBox.getPosition();
        
            pos.x -= 30;
            pos.y -= 2;
            
            mainFont->drawString( translate("disableFlashEffect"), pos, alignRight );
            }
        

        pos = mFullscreenBox.getPosition();
        
        pos.y += 52;
        pos.x -= 16;
        
        if( getCountingOnVsync() ) {
            mainFont->drawString( translate( "vsyncYes" ), pos, alignLeft );
            }
        else {
            mainFont->drawString( translate( "vsyncNo" ), pos, alignLeft );
            }
        
        pos.y += 52;

        char *fpsString = autoSprintf( "%d", targetFramesPerSecond );
        
        mainFont->drawString( fpsString, pos, alignLeft );
        delete [] fpsString;


        pos.y += 52;

        char *currentFPSString = autoSprintf( "%.2f", getRecentFrameRate() );
        
        mainFont->drawString( currentFPSString, pos, alignLeft );
        delete [] currentFPSString;
        

        pos = mFullscreenBox.getPosition();
        pos.x -= 30;

        pos.y += 52;
        mainFont->drawString( translate( "vsyncOn" ), pos, alignRight );
        pos.y += 52;
        mainFont->drawString( translate( "targetFPS" ), pos, alignRight );
        pos.y += 52;
        mainFont->drawString( translate( "currentFPS" ), pos, alignRight );
        }


    if( mEnableNudeBox.isVisible() ) {
        doublePair pos = mEnableNudeBox.getPosition();
        
        pos.x -= 30;
        pos.y -= 2;

        mainFont->drawString( translate("enableNudity"), pos, alignRight );
        }
        
    if( mUseCustomServerBox.isVisible() ) {
        doublePair pos = mUseCustomServerBox.getPosition();
        
        pos.x -= 30;
        pos.y -= 2;

        mainFont->drawString( translate( "useCustomServer" ), pos, alignRight );
        
        if( mUseCustomServerBox.getToggled() ) {
            pos.y += 2;
            pos.y -= 52 + 52/4;
            mainFont->drawString( translate( "address" ), pos, alignRight );
            pos.y -= 52 + 52/4;
            mainFont->drawString( translate( "port" ), pos, alignRight );
            }
        }
	
    if( mEnableFOVBox.isVisible() ) {
        doublePair pos = mEnableFOVBox.getPosition();
        
        pos.x -= 30;
        pos.y -= 2;

        mainFont->drawString( translate("enableZoom"), pos, alignRight );
        }
	
    if( mEnableKActionsBox.isVisible() ) {
        doublePair pos = mEnableKActionsBox.getPosition();
        
        pos.x -= 30;
        pos.y -= 2;

        mainFont->drawString( translate("keyboardActions"), pos, alignRight );
        }
	
    if( mEnableCenterCameraBox.isVisible() ) {
        doublePair pos = mEnableCenterCameraBox.getPosition();
        
        pos.x -= 30;
        pos.y -= 2;

        mainFont->drawString( translate("centerCamera"), pos, alignRight );
        }


    if( mCursorModeSet->isVisible() ) {
        
        double xPos = mEnableCenterCameraBox.getPosition().x - 30;
        
        doublePair pos = mCursorModeSet->getPosition();
        
        pos.x = xPos;
        pos.y += 37;
        
        mainFont->drawString( translate( "cursor"), pos, alignRight );
        
        if( mCursorScaleSlider.isVisible() ) {
            
            pos = mCursorScaleSlider.getPosition();
            
            pos.x = xPos;
            pos.y -= 2;
            
            mainFont->drawString( translate( "scale"), pos, alignRight );
            }
        }
#ifdef USE_DISCORD
    if( mEnableDiscordRichPresence.isVisible() ) {
        doublePair pos = mEnableDiscordRichPresence.getPosition();
        
        pos.x -= 30;
        pos.y -= 2;

        mainFont->drawString( translate("richPresence"), pos, alignRight );
        }
    if( mEnableDiscordRichPresenceStatus.isVisible() ) {
        doublePair pos = mEnableDiscordRichPresenceStatus.getPosition();

        pos.x -= 30;
        pos.y -= 2;

        mainFont->drawString( translate("richPresenceStatus"), pos, alignRight );
        }
    if( mEnableDiscordShowAgeInStatus.isVisible() ) {
        doublePair pos = mEnableDiscordShowAgeInStatus.getPosition();

        pos.x -= 30;
        pos.y -= 2;

        mainFont->drawString( translate("showAge"), pos, alignRight );
        }
    if( mEnableDiscordRichPresenceDetails.isVisible() ) {
        doublePair pos = mEnableDiscordRichPresenceDetails.getPosition();

        pos.x -= 30;
        pos.y -= 2;

        mainFont->drawString( translate("richPresenceDetails"), pos, alignRight );
        }
    if( mDiscordHideFirstNameInDetails.isVisible() ) {
        doublePair pos = mDiscordHideFirstNameInDetails.getPosition();

        pos.x -= 30;
        pos.y -= 2;

        mainFont->drawString( translate("hideFirstName"), pos, alignRight );
        }    
    // prevent someone from making spamming requests to update their status, discord may block our key, due to spamming requests!
    // allow for at least 2 seconds to pass until the user is allowed to change the discord setting again.
    mEnableDiscordRichPresence.setActive(time(0) - last_discord_setting_change > 2);
    mEnableDiscordRichPresenceStatus.setActive(time(0) - last_discord_setting_change > 2);
    mEnableDiscordShowAgeInStatus.setActive(time(0) - last_discord_setting_change > 2);
    mEnableDiscordRichPresenceDetails.setActive(time(0) - last_discord_setting_change > 2);
    mDiscordHideFirstNameInDetails.setActive(time(0) - last_discord_setting_change > 2);
#endif // USE_DISCORD

    if (mEnableAdvancedShowUseOnObjectHoverKeybind.isVisible()) {
        doublePair pos = mEnableAdvancedShowUseOnObjectHoverKeybind.getPosition();
        pos.x -= 30;
        pos.y -= 2;

        mainFont->drawString(translate("showUseOnHover"), pos, alignRight);
    }
}



void SettingsPage::step() {
    if( mTestSound.numSubSounds > 0 ) {
        markSoundUsageLive( mTestSound );
        }
    stepMusicPlayer();
    }





void SettingsPage::makeActive( char inFresh ) {
    if( inFresh ) {        

        int mode = getCursorMode();
        
        mCursorModeSet->setSelectedItem( mode );
        
        mCursorScaleSlider.setValue( getEmulatedCursorScale() );


        int useCustomServer = 
            SettingsManager::getIntSetting( "useCustomServer", 0 );
        
        mUseCustomServerBox.setToggled( useCustomServer );
        

        char *address = 
            SettingsManager::getStringSetting( "customServerAddress",
                                               "localhost" );
        
        int port = SettingsManager::getIntSetting( "customServerPort", 8005 );
        
        mCustomServerAddressField.setText( address );
        mCustomServerPortField.setInt( port );
        
        delete [] address;
        


        mMusicLoudnessSlider.setValue( musicLoudness );
        mSoundEffectsLoudnessSlider.setValue( getSoundEffectsLoudness() );
        setMusicLoudness( 0 );
        mMusicStartTime = 0;
        
        int tryCount = 0;
        
        while( mTestSound.numSubSounds == 0 && tryCount < 10 ) {

            int oID = getRandomPersonObject();

            if( oID > 0 ) {
                ObjectRecord *r = getObject( oID );
                if( r->usingSound.numSubSounds >= 1 ) {
                    mTestSound = copyUsage( r->usingSound );
                    // constrain to only first subsound                    
                    mTestSound.numSubSounds = 1;
                    // play it at full volume
                    mTestSound.volumes[0] = 1.0;
                    }
                }
            tryCount ++;
            }
        
        if( SettingsManager::getIntSetting( "useSteamUpdate", 0 ) ) {
            mEditAccountButton.setVisible( true );
            }
        else {
            mEditAccountButton.setVisible( false );
            }
            
        updatePage();

        }
    }



void SettingsPage::makeNotActive() {
    }


void SettingsPage::updatePage() {

    double lineSpacing = 52;
    
    mEnableFOVBox.setPosition( 0, lineSpacing * 3 );
    mEnableCenterCameraBox.setPosition( 0, lineSpacing * 2 );
    mEnableNudeBox.setPosition( 0, lineSpacing );
    mUseCustomServerBox.setPosition( 0, -lineSpacing );
    mCustomServerAddressField.setPosition( 180 - 16, -lineSpacing * 2 - lineSpacing/4 );
    mCustomServerPortField.setPosition( 180 - 16, -lineSpacing * 3  - lineSpacing/2 );
    mCopyButton.setPadding( 8, 4 );
    mCopyButton.setPosition( 
        mCustomServerAddressField.getRightEdgeX() + mCopyButton.getWidth() / 2 + 12, 
        mCustomServerAddressField.getPosition().y );
    mPasteButton.setPadding( 8, 4 );
    mPasteButton.setPosition(
        mCustomServerAddressField.getRightEdgeX() + 12 + 
        mCopyButton.getWidth() + 
        12 + mPasteButton.getWidth() / 2, 
        mCustomServerAddressField.getPosition().y );
    
    mEnableKActionsBox.setPosition( 0, lineSpacing * 2 );
    mCursorModeSet->setPosition( 0, 0 );
    mCursorScaleSlider.setPosition( -80, -lineSpacing * 3 );
    
    mMusicLoudnessSlider.setPosition( 0, lineSpacing / 2 );
    mSoundEffectsLoudnessSlider.setPosition( 0, - lineSpacing / 2 );
    
    mFullscreenBox.setPosition( 0, -lineSpacing );
    mBorderlessBox.setPosition( 0, -lineSpacing * 2 );
    mTrippingEffectDisabledBox.setPosition( 0, -lineSpacing * 3 );
    mRedetectButton.setPosition( 160, lineSpacing * 2 );
    mRedetectButton.setPadding( 8, 4 );

#ifdef USE_DISCORD
    mEnableDiscordRichPresence.setPosition(0, 3 * lineSpacing);
    mEnableDiscordRichPresenceStatus.setPosition(0, 2 * lineSpacing);
    mEnableDiscordShowAgeInStatus.setPosition(0, lineSpacing);
    mEnableDiscordRichPresenceDetails.setPosition(0, 0);
    mDiscordHideFirstNameInDetails.setPosition(0, -lineSpacing);
#endif // USE_DISCORD

    mEnableAdvancedShowUseOnObjectHoverKeybind.setPosition(0, 3 * lineSpacing);
    
    mEnableFOVBox.setVisible( mPage == 0 );
    mEnableCenterCameraBox.setVisible( mPage == 0 );
    mEnableNudeBox.setVisible( mPage == 0 );
    mUseCustomServerBox.setVisible( mPage == 0 );
    mCustomServerAddressField.setVisible( mPage == 0 && mUseCustomServerBox.getToggled() );
    mCustomServerPortField.setVisible( mPage == 0 && mUseCustomServerBox.getToggled() );
    mCopyButton.setVisible( isClipboardSupported() && mPage == 0 && mUseCustomServerBox.getToggled() );
    mPasteButton.setVisible( isClipboardSupported() && mPage == 0 && mUseCustomServerBox.getToggled() );

    mEnableKActionsBox.setVisible( mPage == 1 );
    mCursorModeSet->setVisible( mPage == 1 );
    int mode = getCursorMode();
    mCursorScaleSlider.setVisible( mode > 0 && mCursorModeSet->isVisible() );
    

    mFullscreenBox.setVisible( mPage == 2 );
    mBorderlessBox.setVisible( mPage == 2 && mFullscreenBox.getToggled() );
    mTrippingEffectDisabledBox.setVisible( mPage == 2 );
    mRedetectButton.setVisible( mPage == 2 );

    mMusicLoudnessSlider.setVisible( mPage == 3 );
    mSoundEffectsLoudnessSlider.setVisible( mPage == 3 );

#ifdef USE_DISCORD
    mEnableDiscordRichPresence.setVisible(mPage == 4);
    mEnableDiscordRichPresenceStatus.setVisible(mPage == 4 && mEnableDiscordRichPresence.getToggled());
    mEnableDiscordShowAgeInStatus.setVisible(mPage == 4 
                                        && mEnableDiscordRichPresence.getToggled()
                                        && mEnableDiscordRichPresenceStatus.getToggled());
    mEnableDiscordRichPresenceDetails.setVisible(mPage == 4 
                                        && mEnableDiscordRichPresence.getToggled() 
                                        && mEnableDiscordRichPresenceStatus.getToggled());
    mDiscordHideFirstNameInDetails.setVisible(mPage == 4 
                                        && mEnableDiscordRichPresenceDetails.getToggled() 
                                        && mEnableDiscordRichPresence.getToggled() 
                                        && mEnableDiscordRichPresenceStatus.getToggled());
#endif // USE_DISCORD

    mEnableAdvancedShowUseOnObjectHoverKeybind.setVisible(mPage == 5);
    
    mGameplayButton.setActive( mPage != 0 );
    mControlButton.setActive( mPage != 1 );
    mScreenButton.setActive( mPage != 2 );
    mSoundButton.setActive( mPage != 3 );

#ifdef USE_DISCORD
    mDiscordButton.setActive( mPage != 4 );
#endif // USE_DISCORD

    mAdvancedButton.setActive( mPage != 5 );

}

void SettingsPage::checkRestartRequired() {
    if( mOldFullscreenSetting != mFullscreenBox.getToggled() ||
        mOldBorderlessSetting != mBorderlessBox.getToggled()
        ) {
        setStatusDirect( translate("restartRequired") , true );
        // Do not show RESTART button when setting page is accessed mid-game
        if ( ! showingInGameSettings )
        mRestartButton.setVisible( true );
        }
    else {
        setStatus( NULL, false );
        mRestartButton.setVisible( false );
        }
    }
