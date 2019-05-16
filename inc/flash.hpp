/**
 * @file flash.hpp
 * @author Alexandre Charrier
 * @version 1.0
 * @date 20 Fev 2019
 * @brief Librarie permettant d'acceder � la m�moire flash (AT45DQ321 30Mbit) via un port SPI pour le fonctionnement du R�volution (Afficheur DEL rotatif).
 *
 * La m�moire flash est configur�e principalement pour contenir et acceder des images BMP 256 x 192 x 16bit.
 * Des donn�es de configuration servant au bon fonctionnement de l'afficheur DEL y sont �galement sauvegard�es.
 *
 * Les images sauvegard�s sont pr�alablement trait�es par l'entremise du serveur WEB du R�volution (afficheur DEL)
 * pour �tre directement transf�r� dans les LED Drivers TLC5955.
 * Elles sont sauvegard�es colonnes par colonnes et elles sont lues de la m�me fa�on.
 *
 */

#ifndef FLASH_HPP_
#define FLASH_HPP_

#include "stm32f4xx_spi.h"
#include "hardware.h"
#include "ispi.hpp"
#include <stdio.h>

class Flash {
public:

	///Positions des images dans le carrousel. Ex:xx00000000000101 = Images 0 et 2 pr�sentes.
	uint32_t positionOfPresentImages = 0;

	///Param�tre de courrant Maximum pour le TLC5955
	uint8_t redMaxCurrent = 127;

	///Param�tre de courrant Maximum pour le TLC5955
	uint8_t greenMaxCurrent = 127;

	///Param�tre de courrant Maximum pour le TLC5955
	uint8_t blueMaxCurrent = 127;

	///Param�tre de courrant Maximum pour le TLC5955
	uint8_t globalBrightness = 127;

	/**
	 * Structure d'address pour acc�der la m�moire flash.
	 * Les 8 191 pages contiennent 512 bytes chacunes.
	 */
	struct address_t {
		uint16_t page = 0;
		uint16_t byte = 0;
	};
	/**
	 *
	 * @param debug : Active la sortie des messages de d�verminage vers la console
	 * @return L'instance de l'objet FLash
	 */
	static Flash* getInstance(bool debug = 0);
	///Images maximums pouvant �tre sauvegard�es dans la flash
	static constexpr uint8_t MaxImageStored = 14;
	///Nombre de caract�res maximum par nom de fichier
	static constexpr uint16_t FilenameSize = 16;
	///Taille du tableau de donn�s envoy� vers trois TLC5955 par port SPI
	static constexpr uint16_t SPIBufferSize = 289;
	///Taille total d'une colonne de 192 pixels
	static constexpr uint16_t ColumnPixelArraySize = 1156;
	///Taille d'un fichier d'image
	static constexpr uint32_t ImageFileSize = 295936;
	///Nombre de pages de m�moire par fichier d'image.
	static constexpr uint16_t PagesPerImage = 578;//Nombre de pages de m�moire par image
	static constexpr uint8_t FirstImagePageAddress = 99;//La page de la premi�re image
	static constexpr uint16_t PageSize = 512;		//Nombre de bytes par pages
	static constexpr uint16_t MaxColumnCount = 256;	//R�solution X maximum du r�volution

	/**
	 * Destructeur de l'objet
	 */
	~Flash();

	/**
	 * Initialise le port SPI5 du STM32F411RE
	 */
	void init();
	/**
	 *
	 * @param data : Donn� en byte � transf�rer dans le port SPI
	 * @return byte lu par le port SPI
	 */
	uint8_t spiTransfer(uint8_t data);
	/**
	 *
	 * @param state : 1 = active le Chip Select,  0 = d�sactive le Chip Select
	 */
	void setCS(bool state);
	/**
	 *
	 * @return busy state. 0 = Flash est pr�t pour une nouvelle op�ration
	 */
	bool isBusy();
	/**
	 *
	 * @return Le registre d'�tat de la flash
	 */
	uint16_t readStatusRegister();
	/**
	 *
	 * @return Le registre de configuration de la flash
	 */
	uint8_t readConfigurationRegister();
	/**
	 * Lie le registre de configuration et l'envoie en format texte dans la console
	 */
	void readStatusRegisterToString();

	/**
	 *
	 * @param add : Adresse de la flash
	 * @param byte : Byte � �crire
	 */
	void write(const address_t *add, uint8_t byte);
	/**
	 *
	 * @param add : Adresse de la flash
	 * @param byte : Int � �crire (2 bytes)
	 */
	void write(const address_t *add, int byte);
	/**
	 *
	 * @param add : Adresse de la flash
	 * @param byte : uint16_t � �crire (2 bytes)
	 */
	void write(const address_t *add, uint16_t byte);
	/**
	 *
	 * @param add : Adresse de la flash
	 * @param byte : uint32_t � �crire (4 bytes)
	 */
	void write(const address_t *add, uint32_t byte);
	/**
	 *
	 * @param add : Adresse de la flash
	 * @param byte : Tableau de uint8_t � transf�rer
	 * @param nByte : nombre de bytes � tranf�rer. � noter que les pages contiennent 512 Bytes.
	 * si l'�criture d�passe le byte 511, l'�criture se continue au d�but de la m�me page au byte 0.
	 * @param offsetByte : D�calage de lecture du tableau � transf�rer.
	 * Ex:  235 signifirait que les donn�s � �crire dans la flash d�buterait � 235e case du tableau (byte[235]).
	 */
	void write(const address_t *add, uint8_t *byte, uint16_t nByte,
			uint16_t offsetByte = 0);
	/**
	 *
	 * @param add : Adresse de la flash
	 * @param byte : Tableau de uint8_t � transf�rer
	 * @param nByte : nombre de bytes � tranf�rer. � noter que les pages contiennent 512 Bytes.
	 * si l'�criture d�passe le byte 511, l'�criture se continuera au d�but de la m�me page au byte 0.
	 */
	void write(const address_t *add, const char *byte, uint16_t nByte);
	/**
	 *
	 * @param add : Adresse de la flash
	 * @return Le byte lu � l'adresse
	 */
	uint8_t read(const address_t *add);
	/**
	 *
	 * @param add : Adresse de la flash
	 * @param buffer : Tableau dans laquelle la lecture sera tranf�r�e.
	 * @param nBytes : nombre de bytes � transf�rer. � noter que les pages contiennent 512 Bytes.
	 * Si la lecture d�passe le byte 511, la lecture se continuera au d�but de la m�me page au byte 0.
	 */
	void read(const address_t *add, uint8_t *buffer, uint16_t nBytes);
	/**
	 *
	 * @param add : Adresse de la flash
	 * @param buffer : Tableau dans laquelle la lecture sera tranf�r�e.
	 * @param nBytes : nombre de bytes � transf�rer. � noter que les pages contiennent 512 Bytes.
	 * Si la lecture d�passe le byte 511, la lecture se continuera au d�but de la m�me page au byte 0.
	 */
	void read(const address_t *add, char *buffer, uint16_t nBytes);
	/**
	 *
	 * @param add : Adresse de la flash
	 * @param buffer : Tableau dans laquelle la lecture sera tranf�r�e.
	 * @param nBytes : nombre de bytes � transf�rer. La lecture se fait en continue et la flash incr�mente
	 * automatiquement l'adresse pour lire les pages pr�c�dantes.
	 */
	void readPageArray(const address_t *add, uint8_t *buffer, uint32_t nByte);
	/**
	 *
	 * @param add : Adresse de la flash
	 *
	 * Efface le contenue de la page � l'adresse sp�cifi�e.
	 */
	void formatPage(const address_t *add);
	/**
	 * Efface la m�moire enti�re de la flash.
	 */
	void formatFlash();
	/**
	 * D�sactive la protection d'�criture s�lective de la flash
	 */
	void disableSectorProtection();
	/**
	 *
	 * @param buffer : Tableau de grandeur minimum de 5 dans lequelle le Device id est retourn�.
	 */
	void getDeviceID(uint8_t *buffer);
	/**
	 * M�thode d'initialisation des formats de page en 512 Bytes par page de m�moire.
	 */
	void setPageSizeBinary();
	/**
	 * Retrouve les donn�es de controle du TLC5955 enregistr�es dans la Flash. Ces donn�es sont emagasin�es
	 * dans les membres de l'objet : redMaxCurrent, greenMaxCurrent, blueMaxCurrent et gloalBrightness.
	 */
	void readControlTLC5955Register();
	/**
	 * Sauvegarde les donn�es de controle du TLC5955 enregistr�es dans la Flash. Ces donn�es proviennent
	 * dess membres de l'objet : redMaxCurrent, greenMaxCurrent, blueMaxCurrent et gloalBrightness.
	 */
	void writeControlTLC5955Register();
	/**
	 *
	 * @return uint16_t contenant les positions d'images pr�sentes dans le carrousel.
	 * Chaque bit repr�sente une image (Max 14 images)
	 * Ex: xx00010000000011 ----> Images 0, 1 et 10 pr�sentes dans le carrousel.
	 */
	uint16_t getPositionOfPresentImagesInCarrousel();
	/**
	 * Sauvegarde le positionnement des images pr�sentes dans le carrousel.
	 * Chaque bit repr�sente une image (Max 14 images)
	 * Ex: xx00010000000011 ----> Images 0, 1 et 10 pr�sentes dans le carrousel.
	 */
	void savePositionOfPresentImagesInCarrousel();
	/**
	 *
	 * @param imageNo : Le no de l'image � rendre disponible au carrousel.
	 *
	 * Cette m�thode n'�crit pas l'image dans la flash.
	 */
	void setImageInCarrousel(uint8_t imageNo);
	/**
	 *
	 * @param imageNo : Le num�ro de l'image � retirer du carrousel.
	 *
	 * Cette m�thode n'�fface pas l'image dans la flash.
	 */
	void resetImageInCarrousel(uint8_t imageNo);
	/**
	 * Retire et rend indiponible toutes les images du carrousel.
	 */
	void formatCarrousel();

	/**
	 *
	 * @return Le nombre d'images disponible dans le carrousel.
	 */
	uint8_t getNumberOfImagesInCarrousel();
	/**
	 *
	 * @return Le prochain emplacement libre pour sauvegader une image.
	 * Si le carrousel est plein, l'image 0 sera �cras�e.
	 */
	uint8_t getNextFreeImageSlot();
	/**
	 *
	 * @param imageNo : Le num�ro d'image.
	 * @param destination : Tableau dans lequel le nom sera transf�r�. Le tableau doit au minimum de FilenameSize.
	 */
	void getFilename(uint8_t imageNo, char *destination);
	/**
	 *
	 * @param imageNo : Le num�ro de l'image.
	 * @param fileName : Tableau du nom du fichier. Si le tableau est plus grad que FilenameSize
	 */
	void setFilename(uint8_t imageNo, uint8_t *fileName);
	void resetFilename(uint8_t imageNo);

	bool getPixelColumn(uint8_t imageNo, uint8_t columnNo, uint8_t* spiBuffer1,
			uint8_t* spiBuffer2, uint8_t* spiBuffer3, uint8_t* spiBuffer4);
	bool savePixelColumn(uint8_t imageNo, uint8_t columnNo, uint8_t* source);
	void getPixelColumnToString(uint8_t imageNo, uint8_t columnNo);
	void setDebug(bool debug);
	uint8_t countSetBits(uint32_t n);

private:
	Flash();
	Flash(bool debugEnable);

	static Flash* instance;

	/**
	 * Flash read/write and SPI command
	 */

	static constexpr uint16_t BusyFlag = 0x8080;
	static constexpr uint16_t ReadWriteErrorFlag = 0x0020;

	static constexpr uint8_t DummyByte = 0x00;

	static constexpr uint8_t MainMemmoryPageRead = 0xD2;
	static constexpr uint8_t ContinuousPageRead = 0x1B;
	static constexpr uint8_t WrtitePagesThroughBuf1BIE = 0x82;
	static constexpr uint8_t WrtitePagesThroughBuf2BIE = 0x85;
	static constexpr uint8_t WrtiteBytesThroughBuf1NoBIE = 0x02;
	static constexpr uint8_t PageErase = 0x81;
	static constexpr uint8_t DeviceID = 0x9F;
	static constexpr uint8_t StatusRegisterRead = 0xD7;
	static constexpr uint8_t ConfigurationRegisterRead = 0x3F;

	static constexpr uint8_t BinaryPageSize[4] = { 0x3D, 0x2A, 0x80, 0xA6 };
	static constexpr uint8_t ChipErase[4] = { 0xC7, 0x94, 0x80, 0x9A };
	static constexpr uint8_t DisableSectorProtect[4] =
			{ 0x3D, 0x2A, 0x7F, 0x9A };

	/**
	 * Static configuration addresses
	 */

	/*
	 * Page 9 is reserved for BMP file names: 15 * 16 char = 240 bytes ex:123456789012.bmp
	 */

	const address_t FilenamePage = { 9, 0 };//15 images * 34 char = 510 bytes
	const address_t GlobalBrightnessSettingAddress = { 10, 0 };		//2 bytes
	const address_t RedMaxCurrentSettingAddress = { 10, 2 };		//2 bytes
	const address_t GreenMaxCurrentSettingAddress = { 10, 4 };		//2 bytes
	const address_t BlueMaxCurrentSettingAddress = { 10, 6 };		//2 Bytes
	const address_t PositionOfPresentImagesInCarrouselAddress = { 10, 7 };//4 bytes

	bool buff1_Buff2 = true;
	bool debug = false;

};

#endif /* FLASH_HPP_ */

