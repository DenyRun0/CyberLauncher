/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *central;
    QHBoxLayout *rootLayout;
    QFrame *sidebar;
    QVBoxLayout *sidebarLayout;
    QLabel *logoLabel;
    QSpacerItem *spacer1;
    QToolButton *navPlay;
    QToolButton *navMods;
    QToolButton *navPacks;
    QToolButton *navSettings;
    QToolButton *navLog;
    QSpacerItem *spacer2;
    QStackedWidget *stack;
    QWidget *pagePlay;
    QVBoxLayout *pagePlayLayout;
    QHBoxLayout *topBarRow;
    QSpacerItem *spacer3;
    QLabel *nickCaption;
    QLineEdit *nickEdit;
    QHBoxLayout *headerRow;
    QVBoxLayout *heroColumn;
    QLabel *heroTitle;
    QHBoxLayout *chipsRow;
    QLabel *chipVersion;
    QLabel *chipJava;
    QLabel *chipPlaytime;
    QLabel *chipLast;
    QSpacerItem *spacer4;
    QSpacerItem *spacer5;
    QPushButton *playButton;
    QProgressBar *progressBar;
    QLabel *statusLabel;
    QHBoxLayout *cardsRow;
    QFrame *cardVersion;
    QVBoxLayout *cardVersionLayout;
    QLabel *versionCaption;
    QComboBox *versionBox;
    QFrame *cardLoader;
    QVBoxLayout *cardLoaderLayout;
    QLabel *loaderCaption;
    QComboBox *loaderBox;
    QSpacerItem *spacer6;
    QWidget *pageMods;
    QVBoxLayout *pageModsLayout;
    QLabel *modsTitle;
    QLabel *modsHint;
    QHBoxLayout *modSearchRow;
    QLineEdit *modSearchEdit;
    QComboBox *modCategoryBox;
    QComboBox *modSortBox;
    QPushButton *modSearchButton;
    QFrame *cardModResults;
    QVBoxLayout *cardModResultsLayout;
    QLabel *modResultsCaption;
    QListWidget *modResults;
    QHBoxLayout *modResultsActions;
    QLabel *modStatusLabel;
    QSpacerItem *spacer7;
    QPushButton *modMoreButton;
    QPushButton *modInstallButton;
    QFrame *cardModInstalled;
    QVBoxLayout *cardModInstalledLayout;
    QLabel *modInstalledCaption;
    QListWidget *installedList;
    QHBoxLayout *modInstalledActions;
    QSpacerItem *spacer8;
    QPushButton *modRemoveButton;
    QWidget *pagePacks;
    QVBoxLayout *pagePacksLayout;
    QLabel *packsTitle;
    QLabel *packsHint;
    QHBoxLayout *packSearchRow;
    QLineEdit *packSearchEdit;
    QComboBox *packSortBox;
    QPushButton *packSearchButton;
    QFrame *cardPackResults;
    QVBoxLayout *cardPackResultsLayout;
    QLabel *packResultsCaption;
    QListWidget *packResults;
    QProgressBar *packProgress;
    QHBoxLayout *packResultsActions;
    QLabel *packStatusLabel;
    QSpacerItem *spacer9;
    QPushButton *packMoreButton;
    QPushButton *packInstallButton;
    QFrame *cardPackInstalled;
    QVBoxLayout *cardPackInstalledLayout;
    QLabel *packInstalledCaption;
    QHBoxLayout *packInstalledBody;
    QListWidget *installedPacks;
    QVBoxLayout *packPreviewColumn;
    QLabel *packPreviewImage;
    QLabel *packPreviewTitle;
    QLabel *packPreviewNote;
    QSpacerItem *spacerPreview;
    QVBoxLayout *packModsColumn;
    QLabel *packModsCaption;
    QListWidget *packModsList;
    QHBoxLayout *packInstalledActions;
    QSpacerItem *spacer10;
    QPushButton *packRemoveButton;
    QWidget *pageSettings;
    QVBoxLayout *pageSettingsLayout;
    QLabel *settingsTitle;
    QHBoxLayout *settingsTopRow;
    QFrame *cardRam;
    QVBoxLayout *cardRamLayout;
    QLabel *ramCaption;
    QSpinBox *ramSpin;
    QLabel *ramHint;
    QFrame *cardVersions;
    QVBoxLayout *cardVersionsLayout;
    QLabel *versionsCaption;
    QCheckBox *snapshotsCheck;
    QPushButton *refreshButton;
    QFrame *cardLanguage;
    QVBoxLayout *cardLanguageLayout;
    QLabel *languageCaption;
    QComboBox *languageBox;
    QLabel *languageHint;
    QFrame *cardFiles;
    QVBoxLayout *cardFilesLayout;
    QLabel *filesCaption;
    QLabel *pathLabel;
    QPushButton *openFolderButton;
    QFrame *cardRecent;
    QVBoxLayout *cardRecentLayout;
    QLabel *recentCaption;
    QLabel *logPreview;
    QWidget *pageLog;
    QVBoxLayout *pageLogLayout;
    QLabel *logTitle;
    QPlainTextEdit *logView;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1040, 680);
        MainWindow->setMinimumSize(QSize(900, 600));
        MainWindow->setStyleSheet(QString::fromUtf8("\n"
"QWidget { color: #f2f2f2; font-size: 14px; }\n"
"QWidget#central { background: transparent; }\n"
"QFrame#sidebar { background-color: rgba(0, 0, 0, 150); border-right: 1px solid rgba(255, 255, 255, 25); }\n"
"QToolButton { background: transparent; border: none; border-radius: 12px; color: #a0a0a0; font-size: 11px; padding: 8px 2px; }\n"
"QToolButton:hover { background-color: rgba(255, 255, 255, 18); color: #ffffff; }\n"
"QToolButton:checked { background-color: rgba(255, 255, 255, 34); color: #ffffff; }\n"
"QFrame[class=\"card\"] { background-color: rgba(8, 8, 8, 175); border: 1px solid rgba(255, 255, 255, 30); border-radius: 16px; }\n"
"QLabel { background: transparent; }\n"
"QLabel[class=\"caption\"] { color: #9a9a9a; font-size: 11px; font-weight: 700; letter-spacing: 1px; }\n"
"QLabel[class=\"muted\"] { color: #b0b0b0; }\n"
"QLabel[class=\"pageTitle\"] { font-size: 30px; font-weight: 700; }\n"
"QLabel#heroTitle { font-size: 42px; font-weight: 700; }\n"
"QLabel[class=\"chip\"] { background-color: rgba(255"
                        ", 255, 255, 26); border-radius: 12px; padding: 5px 12px; color: #e8e8e8; font-size: 12px; }\n"
"QLineEdit, QComboBox, QSpinBox { background-color: #101010; border: 1px solid #333333; border-radius: 10px; padding: 8px 10px; selection-background-color: #ffffff; selection-color: #000000; }\n"
"QLineEdit:focus, QComboBox:focus, QSpinBox:focus { border: 1px solid #ffffff; }\n"
"QComboBox QAbstractItemView { background-color: #101010; border: 1px solid #333333; selection-background-color: #2a2a2a; outline: none; }\n"
"QPushButton { background-color: #1c1c1c; border: 1px solid #333333; border-radius: 10px; padding: 9px 16px; }\n"
"QPushButton:hover { background-color: #2a2a2a; }\n"
"QPushButton#playButton { background-color: #22d65f; color: #03150a; border: none; border-radius: 30px; font-size: 18px; font-weight: 800; padding: 0 36px; }\n"
"QPushButton#playButton:hover { background-color: #3ee878; }\n"
"QPushButton#playButton:disabled { background-color: #1b3a27; color: #6d8f7a; }\n"
"QPushButton#refreshButton { back"
                        "ground-color: #f59e0b; color: #1a1000; border: none; font-weight: 700; }\n"
"QPushButton#refreshButton:hover { background-color: #fbb53a; }\n"
"QPushButton#openFolderButton { background-color: #3b82f6; color: #ffffff; border: none; font-weight: 700; }\n"
"QPushButton#openFolderButton:hover { background-color: #5b9aff; }\n"
"QListWidget { background-color: #0b0b0b; border: 1px solid #2a2a2a; border-radius: 12px; padding: 6px; outline: none; }\n"
"QListWidget::item { padding: 8px 10px; border-radius: 8px; }\n"
"QListWidget::item:selected { background-color: #2a2a2a; color: #ffffff; }\n"
"QListWidget::item:hover { background-color: #181818; }\n"
"QPushButton#modSearchButton { background-color: #3b82f6; color: #ffffff; border: none; font-weight: 700; }\n"
"QPushButton#modSearchButton:hover { background-color: #5b9aff; }\n"
"QPushButton#modInstallButton { background-color: #22d65f; color: #03150a; border: none; font-weight: 800; }\n"
"QPushButton#modInstallButton:hover { background-color: #3ee878; }\n"
"QPushButton"
                        "#modInstallButton:disabled, QPushButton#modSearchButton:disabled { background-color: #262626; color: #6a6a6a; }\n"
"QPushButton#modRemoveButton { background-color: #ef4444; color: #ffffff; border: none; font-weight: 700; }\n"
"QPushButton#modRemoveButton:hover { background-color: #f87171; }\n"
"QPushButton#packSearchButton { background-color: #3b82f6; color: #ffffff; border: none; font-weight: 700; }\n"
"QPushButton#packSearchButton:hover { background-color: #5b9aff; }\n"
"QPushButton#packInstallButton { background-color: #22d65f; color: #03150a; border: none; font-weight: 800; }\n"
"QPushButton#packInstallButton:hover { background-color: #3ee878; }\n"
"QPushButton#packInstallButton:disabled { background-color: #262626; color: #6a6a6a; }\n"
"QPushButton#packRemoveButton { background-color: #ef4444; color: #ffffff; border: none; font-weight: 700; }\n"
"QPushButton#packRemoveButton:hover { background-color: #f87171; }\n"
"QProgressBar { background-color: rgba(255, 255, 255, 30); border: none; border-radius: 4px;"
                        " max-height: 8px; color: transparent; }\n"
"QProgressBar::chunk { background-color: #22d65f; border-radius: 4px; }\n"
"QPlainTextEdit { background-color: #0b0b0b; border: 1px solid #2a2a2a; border-radius: 12px; padding: 8px; font-family: Menlo; font-size: 12px; }\n"
"QCheckBox { spacing: 8px; }\n"
"QCheckBox::indicator { width: 18px; height: 18px; border-radius: 5px; border: 1px solid #555555; background: #101010; }\n"
"QCheckBox::indicator:checked { background-color: #ffffff; border-color: #ffffff; }\n"
"QScrollBar:vertical { background: transparent; width: 10px; }\n"
"QScrollBar::handle:vertical { background: #3a3a3a; border-radius: 5px; min-height: 30px; }\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }\n"
""));
        central = new QWidget(MainWindow);
        central->setObjectName("central");
        rootLayout = new QHBoxLayout(central);
        rootLayout->setSpacing(0);
        rootLayout->setObjectName("rootLayout");
        rootLayout->setContentsMargins(0, 0, 0, 0);
        sidebar = new QFrame(central);
        sidebar->setObjectName("sidebar");
        sidebar->setMinimumSize(QSize(104, 0));
        sidebar->setMaximumSize(QSize(104, 16777215));
        sidebarLayout = new QVBoxLayout(sidebar);
        sidebarLayout->setSpacing(8);
        sidebarLayout->setObjectName("sidebarLayout");
        sidebarLayout->setContentsMargins(10, 18, 10, 18);
        logoLabel = new QLabel(sidebar);
        logoLabel->setObjectName("logoLabel");
        logoLabel->setMinimumSize(QSize(44, 44));
        logoLabel->setMaximumSize(QSize(44, 44));
        logoLabel->setAlignment(Qt::AlignCenter);
        logoLabel->setPixmap(QPixmap(QString::fromUtf8(":/logo.png")));
        logoLabel->setScaledContents(true);

        sidebarLayout->addWidget(logoLabel);

        spacer1 = new QSpacerItem(20, 12, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        sidebarLayout->addItem(spacer1);

        navPlay = new QToolButton(sidebar);
        navPlay->setObjectName("navPlay");
        navPlay->setMinimumSize(QSize(84, 64));
        navPlay->setMaximumSize(QSize(84, 64));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/play.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon.addFile(QString::fromUtf8(":/icons/play_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        navPlay->setIcon(icon);
        navPlay->setIconSize(QSize(26, 26));
        navPlay->setCheckable(true);
        navPlay->setChecked(true);
        navPlay->setAutoExclusive(true);
        navPlay->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        sidebarLayout->addWidget(navPlay);

        navMods = new QToolButton(sidebar);
        navMods->setObjectName("navMods");
        navMods->setMinimumSize(QSize(84, 64));
        navMods->setMaximumSize(QSize(84, 64));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/mods.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon1.addFile(QString::fromUtf8(":/icons/mods_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        navMods->setIcon(icon1);
        navMods->setIconSize(QSize(26, 26));
        navMods->setCheckable(true);
        navMods->setChecked(false);
        navMods->setAutoExclusive(true);
        navMods->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        sidebarLayout->addWidget(navMods);

        navPacks = new QToolButton(sidebar);
        navPacks->setObjectName("navPacks");
        navPacks->setMinimumSize(QSize(84, 64));
        navPacks->setMaximumSize(QSize(84, 64));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/packs.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon2.addFile(QString::fromUtf8(":/icons/packs_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        navPacks->setIcon(icon2);
        navPacks->setIconSize(QSize(26, 26));
        navPacks->setCheckable(true);
        navPacks->setChecked(false);
        navPacks->setAutoExclusive(true);
        navPacks->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        sidebarLayout->addWidget(navPacks);

        navSettings = new QToolButton(sidebar);
        navSettings->setObjectName("navSettings");
        navSettings->setMinimumSize(QSize(84, 64));
        navSettings->setMaximumSize(QSize(84, 64));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/settings.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon3.addFile(QString::fromUtf8(":/icons/settings_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        navSettings->setIcon(icon3);
        navSettings->setIconSize(QSize(26, 26));
        navSettings->setCheckable(true);
        navSettings->setChecked(false);
        navSettings->setAutoExclusive(true);
        navSettings->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        sidebarLayout->addWidget(navSettings);

        navLog = new QToolButton(sidebar);
        navLog->setObjectName("navLog");
        navLog->setMinimumSize(QSize(84, 64));
        navLog->setMaximumSize(QSize(84, 64));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/log.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon4.addFile(QString::fromUtf8(":/icons/log_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        navLog->setIcon(icon4);
        navLog->setIconSize(QSize(26, 26));
        navLog->setCheckable(true);
        navLog->setChecked(false);
        navLog->setAutoExclusive(true);
        navLog->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        sidebarLayout->addWidget(navLog);

        spacer2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        sidebarLayout->addItem(spacer2);


        rootLayout->addWidget(sidebar);

        stack = new QStackedWidget(central);
        stack->setObjectName("stack");
        pagePlay = new QWidget();
        pagePlay->setObjectName("pagePlay");
        pagePlayLayout = new QVBoxLayout(pagePlay);
        pagePlayLayout->setSpacing(16);
        pagePlayLayout->setObjectName("pagePlayLayout");
        pagePlayLayout->setContentsMargins(32, 24, 32, 24);
        topBarRow = new QHBoxLayout();
        topBarRow->setSpacing(12);
        topBarRow->setObjectName("topBarRow");
        spacer3 = new QSpacerItem(40, 10, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        topBarRow->addItem(spacer3);

        nickCaption = new QLabel(pagePlay);
        nickCaption->setObjectName("nickCaption");

        topBarRow->addWidget(nickCaption);

        nickEdit = new QLineEdit(pagePlay);
        nickEdit->setObjectName("nickEdit");
        nickEdit->setMinimumSize(QSize(240, 0));
        nickEdit->setMaximumSize(QSize(280, 16777215));

        topBarRow->addWidget(nickEdit);


        pagePlayLayout->addLayout(topBarRow);

        headerRow = new QHBoxLayout();
        headerRow->setSpacing(16);
        headerRow->setObjectName("headerRow");
        heroColumn = new QVBoxLayout();
        heroColumn->setSpacing(10);
        heroColumn->setObjectName("heroColumn");
        heroTitle = new QLabel(pagePlay);
        heroTitle->setObjectName("heroTitle");

        heroColumn->addWidget(heroTitle);

        chipsRow = new QHBoxLayout();
        chipsRow->setSpacing(8);
        chipsRow->setObjectName("chipsRow");
        chipVersion = new QLabel(pagePlay);
        chipVersion->setObjectName("chipVersion");

        chipsRow->addWidget(chipVersion);

        chipJava = new QLabel(pagePlay);
        chipJava->setObjectName("chipJava");

        chipsRow->addWidget(chipJava);

        chipPlaytime = new QLabel(pagePlay);
        chipPlaytime->setObjectName("chipPlaytime");

        chipsRow->addWidget(chipPlaytime);

        chipLast = new QLabel(pagePlay);
        chipLast->setObjectName("chipLast");

        chipsRow->addWidget(chipLast);

        spacer4 = new QSpacerItem(40, 10, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        chipsRow->addItem(spacer4);


        heroColumn->addLayout(chipsRow);


        headerRow->addLayout(heroColumn);

        spacer5 = new QSpacerItem(40, 10, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerRow->addItem(spacer5);

        playButton = new QPushButton(pagePlay);
        playButton->setObjectName("playButton");
        playButton->setMinimumSize(QSize(240, 60));

        headerRow->addWidget(playButton);


        pagePlayLayout->addLayout(headerRow);

        progressBar = new QProgressBar(pagePlay);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(0);
        progressBar->setTextVisible(false);

        pagePlayLayout->addWidget(progressBar);

        statusLabel = new QLabel(pagePlay);
        statusLabel->setObjectName("statusLabel");

        pagePlayLayout->addWidget(statusLabel);

        cardsRow = new QHBoxLayout();
        cardsRow->setSpacing(16);
        cardsRow->setObjectName("cardsRow");
        cardVersion = new QFrame(pagePlay);
        cardVersion->setObjectName("cardVersion");
        cardVersionLayout = new QVBoxLayout(cardVersion);
        cardVersionLayout->setSpacing(10);
        cardVersionLayout->setObjectName("cardVersionLayout");
        cardVersionLayout->setContentsMargins(18, 16, 18, 18);
        versionCaption = new QLabel(cardVersion);
        versionCaption->setObjectName("versionCaption");

        cardVersionLayout->addWidget(versionCaption);

        versionBox = new QComboBox(cardVersion);
        versionBox->setObjectName("versionBox");

        cardVersionLayout->addWidget(versionBox);


        cardsRow->addWidget(cardVersion);

        cardLoader = new QFrame(pagePlay);
        cardLoader->setObjectName("cardLoader");
        cardLoaderLayout = new QVBoxLayout(cardLoader);
        cardLoaderLayout->setSpacing(10);
        cardLoaderLayout->setObjectName("cardLoaderLayout");
        cardLoaderLayout->setContentsMargins(18, 16, 18, 18);
        loaderCaption = new QLabel(cardLoader);
        loaderCaption->setObjectName("loaderCaption");

        cardLoaderLayout->addWidget(loaderCaption);

        loaderBox = new QComboBox(cardLoader);
        loaderBox->addItem(QString());
        loaderBox->addItem(QString());
        loaderBox->setObjectName("loaderBox");

        cardLoaderLayout->addWidget(loaderBox);


        cardsRow->addWidget(cardLoader);


        pagePlayLayout->addLayout(cardsRow);

        spacer6 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        pagePlayLayout->addItem(spacer6);

        stack->addWidget(pagePlay);
        pageMods = new QWidget();
        pageMods->setObjectName("pageMods");
        pageModsLayout = new QVBoxLayout(pageMods);
        pageModsLayout->setSpacing(12);
        pageModsLayout->setObjectName("pageModsLayout");
        pageModsLayout->setContentsMargins(32, 24, 32, 20);
        modsTitle = new QLabel(pageMods);
        modsTitle->setObjectName("modsTitle");

        pageModsLayout->addWidget(modsTitle);

        modsHint = new QLabel(pageMods);
        modsHint->setObjectName("modsHint");

        pageModsLayout->addWidget(modsHint);

        modSearchRow = new QHBoxLayout();
        modSearchRow->setSpacing(10);
        modSearchRow->setObjectName("modSearchRow");
        modSearchEdit = new QLineEdit(pageMods);
        modSearchEdit->setObjectName("modSearchEdit");

        modSearchRow->addWidget(modSearchEdit);

        modCategoryBox = new QComboBox(pageMods);
        modCategoryBox->addItem(QString());
        modCategoryBox->addItem(QString());
        modCategoryBox->addItem(QString());
        modCategoryBox->addItem(QString());
        modCategoryBox->addItem(QString());
        modCategoryBox->addItem(QString());
        modCategoryBox->addItem(QString());
        modCategoryBox->addItem(QString());
        modCategoryBox->addItem(QString());
        modCategoryBox->setObjectName("modCategoryBox");

        modSearchRow->addWidget(modCategoryBox);

        modSortBox = new QComboBox(pageMods);
        modSortBox->addItem(QString());
        modSortBox->addItem(QString());
        modSortBox->addItem(QString());
        modSortBox->addItem(QString());
        modSortBox->setObjectName("modSortBox");

        modSearchRow->addWidget(modSortBox);

        modSearchButton = new QPushButton(pageMods);
        modSearchButton->setObjectName("modSearchButton");

        modSearchRow->addWidget(modSearchButton);


        pageModsLayout->addLayout(modSearchRow);

        cardModResults = new QFrame(pageMods);
        cardModResults->setObjectName("cardModResults");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cardModResults->sizePolicy().hasHeightForWidth());
        cardModResults->setSizePolicy(sizePolicy);
        cardModResultsLayout = new QVBoxLayout(cardModResults);
        cardModResultsLayout->setSpacing(10);
        cardModResultsLayout->setObjectName("cardModResultsLayout");
        cardModResultsLayout->setContentsMargins(18, 16, 18, 18);
        modResultsCaption = new QLabel(cardModResults);
        modResultsCaption->setObjectName("modResultsCaption");

        cardModResultsLayout->addWidget(modResultsCaption);

        modResults = new QListWidget(cardModResults);
        modResults->setObjectName("modResults");

        cardModResultsLayout->addWidget(modResults);

        modResultsActions = new QHBoxLayout();
        modResultsActions->setSpacing(10);
        modResultsActions->setObjectName("modResultsActions");
        modStatusLabel = new QLabel(cardModResults);
        modStatusLabel->setObjectName("modStatusLabel");

        modResultsActions->addWidget(modStatusLabel);

        spacer7 = new QSpacerItem(40, 10, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        modResultsActions->addItem(spacer7);

        modMoreButton = new QPushButton(cardModResults);
        modMoreButton->setObjectName("modMoreButton");

        modResultsActions->addWidget(modMoreButton);

        modInstallButton = new QPushButton(cardModResults);
        modInstallButton->setObjectName("modInstallButton");
        modInstallButton->setMinimumSize(QSize(160, 40));

        modResultsActions->addWidget(modInstallButton);


        cardModResultsLayout->addLayout(modResultsActions);


        pageModsLayout->addWidget(cardModResults);

        cardModInstalled = new QFrame(pageMods);
        cardModInstalled->setObjectName("cardModInstalled");
        cardModInstalledLayout = new QVBoxLayout(cardModInstalled);
        cardModInstalledLayout->setSpacing(10);
        cardModInstalledLayout->setObjectName("cardModInstalledLayout");
        cardModInstalledLayout->setContentsMargins(18, 16, 18, 18);
        modInstalledCaption = new QLabel(cardModInstalled);
        modInstalledCaption->setObjectName("modInstalledCaption");

        cardModInstalledLayout->addWidget(modInstalledCaption);

        installedList = new QListWidget(cardModInstalled);
        installedList->setObjectName("installedList");
        installedList->setMaximumSize(QSize(16777215, 110));

        cardModInstalledLayout->addWidget(installedList);

        modInstalledActions = new QHBoxLayout();
        modInstalledActions->setSpacing(10);
        modInstalledActions->setObjectName("modInstalledActions");
        spacer8 = new QSpacerItem(40, 10, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        modInstalledActions->addItem(spacer8);

        modRemoveButton = new QPushButton(cardModInstalled);
        modRemoveButton->setObjectName("modRemoveButton");

        modInstalledActions->addWidget(modRemoveButton);


        cardModInstalledLayout->addLayout(modInstalledActions);


        pageModsLayout->addWidget(cardModInstalled);

        stack->addWidget(pageMods);
        pagePacks = new QWidget();
        pagePacks->setObjectName("pagePacks");
        pagePacksLayout = new QVBoxLayout(pagePacks);
        pagePacksLayout->setSpacing(12);
        pagePacksLayout->setObjectName("pagePacksLayout");
        pagePacksLayout->setContentsMargins(32, 24, 32, 20);
        packsTitle = new QLabel(pagePacks);
        packsTitle->setObjectName("packsTitle");

        pagePacksLayout->addWidget(packsTitle);

        packsHint = new QLabel(pagePacks);
        packsHint->setObjectName("packsHint");
        packsHint->setWordWrap(true);

        pagePacksLayout->addWidget(packsHint);

        packSearchRow = new QHBoxLayout();
        packSearchRow->setSpacing(10);
        packSearchRow->setObjectName("packSearchRow");
        packSearchEdit = new QLineEdit(pagePacks);
        packSearchEdit->setObjectName("packSearchEdit");

        packSearchRow->addWidget(packSearchEdit);

        packSortBox = new QComboBox(pagePacks);
        packSortBox->addItem(QString());
        packSortBox->addItem(QString());
        packSortBox->addItem(QString());
        packSortBox->addItem(QString());
        packSortBox->setObjectName("packSortBox");

        packSearchRow->addWidget(packSortBox);

        packSearchButton = new QPushButton(pagePacks);
        packSearchButton->setObjectName("packSearchButton");

        packSearchRow->addWidget(packSearchButton);


        pagePacksLayout->addLayout(packSearchRow);

        cardPackResults = new QFrame(pagePacks);
        cardPackResults->setObjectName("cardPackResults");
        sizePolicy.setHeightForWidth(cardPackResults->sizePolicy().hasHeightForWidth());
        cardPackResults->setSizePolicy(sizePolicy);
        cardPackResultsLayout = new QVBoxLayout(cardPackResults);
        cardPackResultsLayout->setSpacing(10);
        cardPackResultsLayout->setObjectName("cardPackResultsLayout");
        cardPackResultsLayout->setContentsMargins(18, 16, 18, 18);
        packResultsCaption = new QLabel(cardPackResults);
        packResultsCaption->setObjectName("packResultsCaption");

        cardPackResultsLayout->addWidget(packResultsCaption);

        packResults = new QListWidget(cardPackResults);
        packResults->setObjectName("packResults");

        cardPackResultsLayout->addWidget(packResults);

        packProgress = new QProgressBar(cardPackResults);
        packProgress->setObjectName("packProgress");
        packProgress->setValue(0);
        packProgress->setTextVisible(false);

        cardPackResultsLayout->addWidget(packProgress);

        packResultsActions = new QHBoxLayout();
        packResultsActions->setSpacing(10);
        packResultsActions->setObjectName("packResultsActions");
        packStatusLabel = new QLabel(cardPackResults);
        packStatusLabel->setObjectName("packStatusLabel");

        packResultsActions->addWidget(packStatusLabel);

        spacer9 = new QSpacerItem(40, 10, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        packResultsActions->addItem(spacer9);

        packMoreButton = new QPushButton(cardPackResults);
        packMoreButton->setObjectName("packMoreButton");

        packResultsActions->addWidget(packMoreButton);

        packInstallButton = new QPushButton(cardPackResults);
        packInstallButton->setObjectName("packInstallButton");
        packInstallButton->setMinimumSize(QSize(190, 40));

        packResultsActions->addWidget(packInstallButton);


        cardPackResultsLayout->addLayout(packResultsActions);


        pagePacksLayout->addWidget(cardPackResults);

        cardPackInstalled = new QFrame(pagePacks);
        cardPackInstalled->setObjectName("cardPackInstalled");
        cardPackInstalledLayout = new QVBoxLayout(cardPackInstalled);
        cardPackInstalledLayout->setSpacing(10);
        cardPackInstalledLayout->setObjectName("cardPackInstalledLayout");
        cardPackInstalledLayout->setContentsMargins(18, 16, 18, 18);
        packInstalledCaption = new QLabel(cardPackInstalled);
        packInstalledCaption->setObjectName("packInstalledCaption");

        cardPackInstalledLayout->addWidget(packInstalledCaption);

        packInstalledBody = new QHBoxLayout();
        packInstalledBody->setSpacing(14);
        packInstalledBody->setObjectName("packInstalledBody");
        installedPacks = new QListWidget(cardPackInstalled);
        installedPacks->setObjectName("installedPacks");
        installedPacks->setMinimumSize(QSize(0, 150));
        installedPacks->setMaximumSize(QSize(16777215, 170));

        packInstalledBody->addWidget(installedPacks);

        packPreviewColumn = new QVBoxLayout();
        packPreviewColumn->setSpacing(6);
        packPreviewColumn->setObjectName("packPreviewColumn");
        packPreviewImage = new QLabel(cardPackInstalled);
        packPreviewImage->setObjectName("packPreviewImage");
        packPreviewImage->setMinimumSize(QSize(112, 112));
        packPreviewImage->setMaximumSize(QSize(112, 112));
        packPreviewImage->setAlignment(Qt::AlignCenter);

        packPreviewColumn->addWidget(packPreviewImage);

        packPreviewTitle = new QLabel(cardPackInstalled);
        packPreviewTitle->setObjectName("packPreviewTitle");
        packPreviewTitle->setMaximumSize(QSize(180, 16777215));
        packPreviewTitle->setWordWrap(true);
        packPreviewTitle->setStyleSheet(QString::fromUtf8("font-weight: 700;"));

        packPreviewColumn->addWidget(packPreviewTitle);

        packPreviewNote = new QLabel(cardPackInstalled);
        packPreviewNote->setObjectName("packPreviewNote");
        packPreviewNote->setMaximumSize(QSize(180, 16777215));
        packPreviewNote->setWordWrap(true);
        packPreviewNote->setStyleSheet(QString::fromUtf8("font-size: 11px;"));

        packPreviewColumn->addWidget(packPreviewNote);

        spacerPreview = new QSpacerItem(20, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        packPreviewColumn->addItem(spacerPreview);


        packInstalledBody->addLayout(packPreviewColumn);

        packModsColumn = new QVBoxLayout();
        packModsColumn->setSpacing(6);
        packModsColumn->setObjectName("packModsColumn");
        packModsCaption = new QLabel(cardPackInstalled);
        packModsCaption->setObjectName("packModsCaption");

        packModsColumn->addWidget(packModsCaption);

        packModsList = new QListWidget(cardPackInstalled);
        packModsList->setObjectName("packModsList");
        packModsList->setMinimumSize(QSize(0, 130));
        packModsList->setMaximumSize(QSize(16777215, 150));

        packModsColumn->addWidget(packModsList);


        packInstalledBody->addLayout(packModsColumn);

        packInstalledBody->setStretch(0, 2);
        packInstalledBody->setStretch(2, 3);

        cardPackInstalledLayout->addLayout(packInstalledBody);

        packInstalledActions = new QHBoxLayout();
        packInstalledActions->setSpacing(10);
        packInstalledActions->setObjectName("packInstalledActions");
        spacer10 = new QSpacerItem(40, 10, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        packInstalledActions->addItem(spacer10);

        packRemoveButton = new QPushButton(cardPackInstalled);
        packRemoveButton->setObjectName("packRemoveButton");

        packInstalledActions->addWidget(packRemoveButton);


        cardPackInstalledLayout->addLayout(packInstalledActions);


        pagePacksLayout->addWidget(cardPackInstalled);

        stack->addWidget(pagePacks);
        pageSettings = new QWidget();
        pageSettings->setObjectName("pageSettings");
        pageSettingsLayout = new QVBoxLayout(pageSettings);
        pageSettingsLayout->setSpacing(16);
        pageSettingsLayout->setObjectName("pageSettingsLayout");
        pageSettingsLayout->setContentsMargins(32, 28, 32, 24);
        settingsTitle = new QLabel(pageSettings);
        settingsTitle->setObjectName("settingsTitle");

        pageSettingsLayout->addWidget(settingsTitle);

        settingsTopRow = new QHBoxLayout();
        settingsTopRow->setSpacing(16);
        settingsTopRow->setObjectName("settingsTopRow");
        cardRam = new QFrame(pageSettings);
        cardRam->setObjectName("cardRam");
        cardRamLayout = new QVBoxLayout(cardRam);
        cardRamLayout->setSpacing(10);
        cardRamLayout->setObjectName("cardRamLayout");
        cardRamLayout->setContentsMargins(18, 16, 18, 18);
        ramCaption = new QLabel(cardRam);
        ramCaption->setObjectName("ramCaption");

        cardRamLayout->addWidget(ramCaption);

        ramSpin = new QSpinBox(cardRam);
        ramSpin->setObjectName("ramSpin");
        ramSpin->setMinimum(1);
        ramSpin->setMaximum(16);
        ramSpin->setValue(2);

        cardRamLayout->addWidget(ramSpin);

        ramHint = new QLabel(cardRam);
        ramHint->setObjectName("ramHint");
        ramHint->setWordWrap(true);

        cardRamLayout->addWidget(ramHint);


        settingsTopRow->addWidget(cardRam);

        cardVersions = new QFrame(pageSettings);
        cardVersions->setObjectName("cardVersions");
        cardVersionsLayout = new QVBoxLayout(cardVersions);
        cardVersionsLayout->setSpacing(12);
        cardVersionsLayout->setObjectName("cardVersionsLayout");
        cardVersionsLayout->setContentsMargins(18, 16, 18, 18);
        versionsCaption = new QLabel(cardVersions);
        versionsCaption->setObjectName("versionsCaption");

        cardVersionsLayout->addWidget(versionsCaption);

        snapshotsCheck = new QCheckBox(cardVersions);
        snapshotsCheck->setObjectName("snapshotsCheck");

        cardVersionsLayout->addWidget(snapshotsCheck);

        refreshButton = new QPushButton(cardVersions);
        refreshButton->setObjectName("refreshButton");

        cardVersionsLayout->addWidget(refreshButton);


        settingsTopRow->addWidget(cardVersions);

        cardLanguage = new QFrame(pageSettings);
        cardLanguage->setObjectName("cardLanguage");
        cardLanguageLayout = new QVBoxLayout(cardLanguage);
        cardLanguageLayout->setSpacing(10);
        cardLanguageLayout->setObjectName("cardLanguageLayout");
        cardLanguageLayout->setContentsMargins(18, 16, 18, 18);
        languageCaption = new QLabel(cardLanguage);
        languageCaption->setObjectName("languageCaption");

        cardLanguageLayout->addWidget(languageCaption);

        languageBox = new QComboBox(cardLanguage);
        languageBox->setObjectName("languageBox");

        cardLanguageLayout->addWidget(languageBox);

        languageHint = new QLabel(cardLanguage);
        languageHint->setObjectName("languageHint");
        languageHint->setWordWrap(true);

        cardLanguageLayout->addWidget(languageHint);


        settingsTopRow->addWidget(cardLanguage);


        pageSettingsLayout->addLayout(settingsTopRow);

        cardFiles = new QFrame(pageSettings);
        cardFiles->setObjectName("cardFiles");
        cardFilesLayout = new QVBoxLayout(cardFiles);
        cardFilesLayout->setSpacing(12);
        cardFilesLayout->setObjectName("cardFilesLayout");
        cardFilesLayout->setContentsMargins(18, 16, 18, 18);
        filesCaption = new QLabel(cardFiles);
        filesCaption->setObjectName("filesCaption");

        cardFilesLayout->addWidget(filesCaption);

        pathLabel = new QLabel(cardFiles);
        pathLabel->setObjectName("pathLabel");
        pathLabel->setWordWrap(true);

        cardFilesLayout->addWidget(pathLabel);

        openFolderButton = new QPushButton(cardFiles);
        openFolderButton->setObjectName("openFolderButton");

        cardFilesLayout->addWidget(openFolderButton);


        pageSettingsLayout->addWidget(cardFiles);

        cardRecent = new QFrame(pageSettings);
        cardRecent->setObjectName("cardRecent");
        sizePolicy.setHeightForWidth(cardRecent->sizePolicy().hasHeightForWidth());
        cardRecent->setSizePolicy(sizePolicy);
        cardRecentLayout = new QVBoxLayout(cardRecent);
        cardRecentLayout->setSpacing(10);
        cardRecentLayout->setObjectName("cardRecentLayout");
        cardRecentLayout->setContentsMargins(18, 16, 18, 18);
        recentCaption = new QLabel(cardRecent);
        recentCaption->setObjectName("recentCaption");

        cardRecentLayout->addWidget(recentCaption);

        logPreview = new QLabel(cardRecent);
        logPreview->setObjectName("logPreview");
        logPreview->setWordWrap(true);
        logPreview->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        cardRecentLayout->addWidget(logPreview);


        pageSettingsLayout->addWidget(cardRecent);

        stack->addWidget(pageSettings);
        pageLog = new QWidget();
        pageLog->setObjectName("pageLog");
        pageLogLayout = new QVBoxLayout(pageLog);
        pageLogLayout->setSpacing(16);
        pageLogLayout->setObjectName("pageLogLayout");
        pageLogLayout->setContentsMargins(32, 28, 32, 24);
        logTitle = new QLabel(pageLog);
        logTitle->setObjectName("logTitle");

        pageLogLayout->addWidget(logTitle);

        logView = new QPlainTextEdit(pageLog);
        logView->setObjectName("logView");
        logView->setReadOnly(true);

        pageLogLayout->addWidget(logView);

        stack->addWidget(pageLog);

        rootLayout->addWidget(stack);

        MainWindow->setCentralWidget(central);

        retranslateUi(MainWindow);

        stack->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Cyber Launcher", nullptr));
        logoLabel->setText(QString());
#if QT_CONFIG(tooltip)
        navPlay->setToolTip(QCoreApplication::translate("MainWindow", "\320\223\321\200\320\260\321\202\320\270", nullptr));
#endif // QT_CONFIG(tooltip)
        navPlay->setText(QCoreApplication::translate("MainWindow", "\320\223\321\200\320\260\321\202\320\270", nullptr));
#if QT_CONFIG(tooltip)
        navMods->setToolTip(QCoreApplication::translate("MainWindow", "\320\234\320\276\320\264\320\270", nullptr));
#endif // QT_CONFIG(tooltip)
        navMods->setText(QCoreApplication::translate("MainWindow", "\320\234\320\276\320\264\320\270", nullptr));
#if QT_CONFIG(tooltip)
        navPacks->setToolTip(QCoreApplication::translate("MainWindow", "\320\227\320\261\321\226\321\200\320\272\320\270", nullptr));
#endif // QT_CONFIG(tooltip)
        navPacks->setText(QCoreApplication::translate("MainWindow", "\320\227\320\261\321\226\321\200\320\272\320\270", nullptr));
#if QT_CONFIG(tooltip)
        navSettings->setToolTip(QCoreApplication::translate("MainWindow", "\320\235\320\260\320\273\320\260\321\210\321\202\321\203\320\262\320\260\320\275\320\275\321\217", nullptr));
#endif // QT_CONFIG(tooltip)
        navSettings->setText(QCoreApplication::translate("MainWindow", "\320\235\320\260\320\273\320\260\321\210\321\202\321\203\320\262\320\260\320\275\320\275\321\217", nullptr));
#if QT_CONFIG(tooltip)
        navLog->setToolTip(QCoreApplication::translate("MainWindow", "\320\226\321\203\321\200\320\275\320\260\320\273", nullptr));
#endif // QT_CONFIG(tooltip)
        navLog->setText(QCoreApplication::translate("MainWindow", "\320\226\321\203\321\200\320\275\320\260\320\273", nullptr));
        nickCaption->setText(QCoreApplication::translate("MainWindow", "\320\235\320\206\320\232\320\235\320\225\320\231\320\234", nullptr));
        nickCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        heroTitle->setText(QCoreApplication::translate("MainWindow", "Minecraft", nullptr));
        chipVersion->setText(QCoreApplication::translate("MainWindow", "\320\222\320\265\321\200\321\201\321\226\321\217: -", nullptr));
        chipVersion->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "chip", nullptr)));
        chipJava->setText(QCoreApplication::translate("MainWindow", "Java: \320\260\320\262\321\202\320\276", nullptr));
        chipJava->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "chip", nullptr)));
        chipPlaytime->setText(QCoreApplication::translate("MainWindow", "\320\247\320\260\321\201 \321\203 \320\263\321\200\321\226: 0 \321\205\320\262", nullptr));
        chipPlaytime->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "chip", nullptr)));
        chipLast->setText(QCoreApplication::translate("MainWindow", "\320\236\321\201\321\202\320\260\320\275\320\275\321\217 \320\263\321\200\320\260: \321\211\320\265 \320\275\320\265 \320\263\321\200\320\260\320\262", nullptr));
        chipLast->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "chip", nullptr)));
        playButton->setText(QCoreApplication::translate("MainWindow", "\320\223\320\240\320\220\320\242\320\230", nullptr));
        statusLabel->setText(QString());
        statusLabel->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "muted", nullptr)));
        cardVersion->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "card", nullptr)));
        versionCaption->setText(QCoreApplication::translate("MainWindow", "\320\222\320\225\320\240\320\241\320\206\320\257", nullptr));
        versionCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        cardLoader->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "card", nullptr)));
        loaderCaption->setText(QCoreApplication::translate("MainWindow", "\320\227\320\220\320\222\320\220\320\235\320\242\320\220\320\226\320\243\320\222\320\220\320\247", nullptr));
        loaderCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        loaderBox->setItemText(0, QCoreApplication::translate("MainWindow", "Vanilla (\320\261\320\265\320\267 \320\274\320\276\320\264\321\226\320\262)", nullptr));
        loaderBox->setItemText(1, QCoreApplication::translate("MainWindow", "Fabric (\320\267 \320\274\320\276\320\264\320\260\320\274\320\270)", nullptr));

        modsTitle->setText(QCoreApplication::translate("MainWindow", "\320\234\320\276\320\264\320\270", nullptr));
        modsTitle->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "pageTitle", nullptr)));
        modsHint->setText(QString());
        modsHint->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "muted", nullptr)));
        modSearchEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\237\320\276\321\210\321\203\320\272 \320\274\320\276\320\264\321\226\320\262 \320\275\320\260 Modrinth...", nullptr));
        modCategoryBox->setItemText(0, QCoreApplication::translate("MainWindow", "\320\243\321\201\321\226 \320\272\320\260\321\202\320\265\320\263\320\276\321\200\321\226\321\227", nullptr));
        modCategoryBox->setItemText(1, QCoreApplication::translate("MainWindow", "\320\236\320\277\321\202\320\270\320\274\321\226\320\267\320\260\321\206\321\226\321\217", nullptr));
        modCategoryBox->setItemText(2, QCoreApplication::translate("MainWindow", "\320\243\321\202\320\270\320\273\321\226\321\202\320\270", nullptr));
        modCategoryBox->setItemText(3, QCoreApplication::translate("MainWindow", "\320\237\321\200\320\270\320\263\320\276\320\264\320\270", nullptr));
        modCategoryBox->setItemText(4, QCoreApplication::translate("MainWindow", "\320\234\320\260\320\263\321\226\321\217", nullptr));
        modCategoryBox->setItemText(5, QCoreApplication::translate("MainWindow", "\320\242\320\265\321\205\320\275\320\276\320\273\320\276\320\263\321\226\321\227", nullptr));
        modCategoryBox->setItemText(6, QCoreApplication::translate("MainWindow", "\320\224\320\265\320\272\320\276\321\200", nullptr));
        modCategoryBox->setItemText(7, QCoreApplication::translate("MainWindow", "\320\223\320\265\320\275\320\265\321\200\320\260\321\206\321\226\321\217 \321\201\320\262\321\226\321\202\321\203", nullptr));
        modCategoryBox->setItemText(8, QCoreApplication::translate("MainWindow", "\320\221\321\226\320\261\320\273\321\226\320\276\321\202\320\265\320\272\320\270", nullptr));

        modSortBox->setItemText(0, QCoreApplication::translate("MainWindow", "\320\237\320\276\320\277\321\203\320\273\321\217\321\200\320\275\321\226", nullptr));
        modSortBox->setItemText(1, QCoreApplication::translate("MainWindow", "\320\227\320\260 \320\267\320\261\321\226\320\263\320\276\320\274", nullptr));
        modSortBox->setItemText(2, QCoreApplication::translate("MainWindow", "\320\235\320\276\320\262\321\226", nullptr));
        modSortBox->setItemText(3, QCoreApplication::translate("MainWindow", "\320\236\320\275\320\276\320\262\320\273\320\265\320\275\321\226", nullptr));

        modSearchButton->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\321\210\321\203\320\272", nullptr));
        cardModResults->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "card", nullptr)));
        modResultsCaption->setText(QCoreApplication::translate("MainWindow", "\320\232\320\220\320\242\320\220\320\233\320\236\320\223 \320\234\320\236\320\224\320\206\320\222", nullptr));
        modResultsCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        modStatusLabel->setText(QString());
        modStatusLabel->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "muted", nullptr)));
        modMoreButton->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\272\320\260\320\267\320\260\321\202\320\270 \321\211\320\265", nullptr));
        modInstallButton->setText(QCoreApplication::translate("MainWindow", "\320\222\321\201\321\202\320\260\320\275\320\276\320\262\320\270\321\202\320\270", nullptr));
        cardModInstalled->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "card", nullptr)));
        modInstalledCaption->setText(QCoreApplication::translate("MainWindow", "\320\222\320\241\320\242\320\220\320\235\320\236\320\222\320\233\320\225\320\235\320\206 \320\234\320\236\320\224\320\230", nullptr));
        modInstalledCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        modRemoveButton->setText(QCoreApplication::translate("MainWindow", "\320\222\320\270\320\264\320\260\320\273\320\270\321\202\320\270 \320\262\320\270\320\261\321\200\320\260\320\275\320\270\320\271", nullptr));
        packsTitle->setText(QCoreApplication::translate("MainWindow", "\320\227\320\261\321\226\321\200\320\272\320\270", nullptr));
        packsTitle->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "pageTitle", nullptr)));
        packsHint->setText(QCoreApplication::translate("MainWindow", "\320\223\320\276\321\202\320\276\320\262\321\226 \320\267\320\261\321\226\321\200\320\272\320\270 \320\267 Modrinth (Fabric, Minecraft 1.19+). \320\237\321\226\321\201\320\273\321\217 \320\262\321\201\321\202\320\260\320\275\320\276\320\262\320\273\320\265\320\275\320\275\321\217 \320\267\320\261\321\226\321\200\320\272\320\260 \320\267'\321\217\320\262\320\273\321\217\321\224\321\202\321\214\321\201\321\217 \321\203 \321\201\320\277\320\270\321\201\320\272\321\203 \320\262\320\265\321\200\321\201\321\226\320\271 \320\275\320\260 \320\263\320\276\320\273\320\276\320\262\320\275\321\226\320\271 \321\201\321\202\320\276\321\200\321\226\320\275\321\206\321\226.", nullptr));
        packsHint->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "muted", nullptr)));
        packSearchEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\237\320\276\321\210\321\203\320\272 \320\267\320\261\321\226\321\200\320\276\320\272 \320\275\320\260 Modrinth...", nullptr));
        packSortBox->setItemText(0, QCoreApplication::translate("MainWindow", "\320\237\320\276\320\277\321\203\320\273\321\217\321\200\320\275\321\226", nullptr));
        packSortBox->setItemText(1, QCoreApplication::translate("MainWindow", "\320\227\320\260 \320\267\320\261\321\226\320\263\320\276\320\274", nullptr));
        packSortBox->setItemText(2, QCoreApplication::translate("MainWindow", "\320\235\320\276\320\262\321\226", nullptr));
        packSortBox->setItemText(3, QCoreApplication::translate("MainWindow", "\320\236\320\275\320\276\320\262\320\273\320\265\320\275\321\226", nullptr));

        packSearchButton->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\321\210\321\203\320\272", nullptr));
        cardPackResults->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "card", nullptr)));
        packResultsCaption->setText(QCoreApplication::translate("MainWindow", "\320\232\320\220\320\242\320\220\320\233\320\236\320\223 \320\227\320\221\320\206\320\240\320\236\320\232", nullptr));
        packResultsCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        packStatusLabel->setText(QString());
        packStatusLabel->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "muted", nullptr)));
        packMoreButton->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\272\320\260\320\267\320\260\321\202\320\270 \321\211\320\265", nullptr));
        packInstallButton->setText(QCoreApplication::translate("MainWindow", "\320\222\321\201\321\202\320\260\320\275\320\276\320\262\320\270\321\202\320\270 \320\267\320\261\321\226\321\200\320\272\321\203", nullptr));
        cardPackInstalled->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "card", nullptr)));
        packInstalledCaption->setText(QCoreApplication::translate("MainWindow", "\320\222\320\241\320\242\320\220\320\235\320\236\320\222\320\233\320\225\320\235\320\206 \320\227\320\221\320\206\320\240\320\232\320\230", nullptr));
        packInstalledCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        packPreviewImage->setText(QString());
        packPreviewTitle->setText(QString());
        packPreviewNote->setText(QString());
        packPreviewNote->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "muted", nullptr)));
        packModsCaption->setText(QCoreApplication::translate("MainWindow", "\320\234\320\236\320\224\320\230 \320\243 \320\227\320\221\320\206\320\240\320\246\320\206", nullptr));
        packModsCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        packRemoveButton->setText(QCoreApplication::translate("MainWindow", "\320\222\320\270\320\264\320\260\320\273\320\270\321\202\320\270 \320\267\320\261\321\226\321\200\320\272\321\203", nullptr));
        settingsTitle->setText(QCoreApplication::translate("MainWindow", "\320\235\320\260\320\273\320\260\321\210\321\202\321\203\320\262\320\260\320\275\320\275\321\217", nullptr));
        settingsTitle->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "pageTitle", nullptr)));
        cardRam->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "card", nullptr)));
        ramCaption->setText(QCoreApplication::translate("MainWindow", "\320\237\320\220\320\234'\320\257\320\242\320\254 \320\224\320\233\320\257 \320\223\320\240\320\230 (\320\223\320\221)", nullptr));
        ramCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        ramHint->setText(QCoreApplication::translate("MainWindow", "\320\241\320\272\321\226\320\273\321\214\320\272\320\270 \320\276\320\277\320\265\321\200\320\260\321\202\320\270\320\262\320\275\320\276\321\227 \320\277\320\260\320\274'\321\217\321\202\321\226 \320\262\320\270\320\264\321\226\320\273\320\270\321\202\320\270 Minecraft", nullptr));
        ramHint->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "muted", nullptr)));
        cardVersions->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "card", nullptr)));
        versionsCaption->setText(QCoreApplication::translate("MainWindow", "\320\241\320\237\320\230\320\241\320\236\320\232 \320\222\320\225\320\240\320\241\320\206\320\231", nullptr));
        versionsCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        snapshotsCheck->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\272\320\260\320\267\321\203\320\262\320\260\321\202\320\270 \321\201\320\275\320\260\320\277\321\210\320\276\321\202\320\270", nullptr));
        refreshButton->setText(QCoreApplication::translate("MainWindow", "\320\236\320\275\320\276\320\262\320\270\321\202\320\270 \321\201\320\277\320\270\321\201\320\276\320\272", nullptr));
        cardLanguage->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "card", nullptr)));
        languageCaption->setText(QCoreApplication::translate("MainWindow", "\320\234\320\236\320\222\320\220", nullptr));
        languageCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        languageHint->setText(QCoreApplication::translate("MainWindow", "\320\234\320\276\320\262\320\260 \321\226\320\275\321\202\320\265\321\200\321\204\320\265\320\271\321\201\321\203 \320\273\320\260\321\203\320\275\321\207\320\265\321\200\320\260. \320\227\320\260\321\201\321\202\320\276\321\201\320\276\320\262\321\203\321\224\321\202\321\214\321\201\321\217 \320\276\320\264\321\200\320\260\320\267\321\203.", nullptr));
        languageHint->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "muted", nullptr)));
        cardFiles->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "card", nullptr)));
        filesCaption->setText(QCoreApplication::translate("MainWindow", "\320\244\320\220\320\231\320\233\320\230 \320\223\320\240\320\230", nullptr));
        filesCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        pathLabel->setText(QString());
        pathLabel->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "muted", nullptr)));
        openFolderButton->setText(QCoreApplication::translate("MainWindow", "\320\222\321\226\320\264\320\272\321\200\320\270\321\202\320\270 \320\277\320\260\320\277\320\272\321\203 \320\267 \320\263\321\200\320\276\321\216", nullptr));
        cardRecent->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "card", nullptr)));
        recentCaption->setText(QCoreApplication::translate("MainWindow", "\320\236\320\241\320\242\320\220\320\235\320\235\320\206 \320\237\320\236\320\224\320\206\320\207", nullptr));
        recentCaption->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "caption", nullptr)));
        logPreview->setText(QString());
        logPreview->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "muted", nullptr)));
        logTitle->setText(QCoreApplication::translate("MainWindow", "\320\226\321\203\321\200\320\275\320\260\320\273", nullptr));
        logTitle->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "pageTitle", nullptr)));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
