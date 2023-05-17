GeoUTools
Tools für die Verwendung der UII+ Cartridge mit GEOS

Inhaltsverzeichnis

Versionsgeschichte und Download
Firmware- und Einstellungsvoraussetzungen und bekannte Probleme
Einleitung
Was befindet sich auf der Diskette
GeoUConfig Anleitung
GeoUTime Anleitung
GeoUMount Anleitung
Credits

Versionsgeschichte und Download

Version 1.0 - 20230516-1124:
- Offizielle Version 1.0
- Aktualisierte Dokumentation von Version 0.1 auf Version 1.0

Version 0.3 - 20230512-1251:
- Wieder viele Bugfixes
- Der Name von GeoUTimeCfg wurde in GeoUConfig geändert, da nun auch Konfigurationseinstellungen für GeoUMount festgelegt werden können
- Über GeoUConfig kann nun die automatische Erkennung gültiger Ziele von GeoUMount manuell geändert werden, wenn es zu älterenProblemen oder Problemen mit der automatischen Erkennung kommt. Auch das Standard-Ziellaufwerk kann vom Benutzer geändert werden.

Version 0.2 - 20230423-1542:
- Viele Bugfixes und Code-Verbesserungen, die Probleme beheben, die dazu führten, dass die Anwendungen in vielen GEOS-Konfigurationen abstürzten
- ZusätzlicheUnterstützung für neue Firmware-Funktionen zur Erkennung von GEOS-RAM-Laufwerken, die das Einbinden und Speichern von Images über das Ultimate Command Interface unterstützen.
- Bessere Ausfallsicherungen für ältere Firnwares

Version 0.1 - 20230128 - 2223:
- Erste öffentliche Alpha-Version


Firmware- und Einstellungsvoraussetzungen und bekannte Probleme

Ultimate II/II+/II+L Firmware-Anforderungen
- Firmware 3.10f oder neuer: vollautomatische Laufwerkserkennung und Unterstützung zum Erkennen/Mounten/Speichern von GEOS-RAM-Drives
- Firmware 3.xx: automatische Geräteerkennung möglich. In Firmwares, die älter als 3.10f sind, basiert die automatische Geräteerkennung jedoch auf den Hardware-Laufwerks-IDs der emulierten A- und B-Laufwerke der Ultimate II/II+/II+L-Kartusche und nicht auf der Software-ID. Da in vielen GEOS-Konfigurationen die Software-IDs von Laufwerken von GEOS geändert werden, kann dies zu Problemen bei der Geräteerkennung führen, die schief gehen und als Reaktion darauf reagieren. Unerwartete Ergebnisse beim Einbinden von Images auf diese Laufwerke. Um dies zu beheben, wählen Sie die manuelle Überschreibung der automatisch erkannten Ziele mit GeoUConfig.
- Firmwares vor 3.xx: Eine automatische Zielerkennung ist nicht möglich, daher ist immer das Setzen gültiger Laufwerksziele über GeoUConfig erforderlich, damit GeoUMount funktioniert.

Link zu Ultimate II/II+/II+L Firmwares:
- Offiziell veröffentlichte Versionen: https://ultimate64.com/Firmware
- Github: https://github.com/GideonZ/ultimate_releases (kann neuere, noch nicht offiziell veröffentlichte Firmwares oder Firmwares enthalten, die vorübergehend zurückgezogen wurden)

Ultimative II/II+/II+L-Einstellanforderungen

Für GeoUTools ist es wichtig, diese Einstellungen im Menü 'C64 and Cartridge Settings' zu überprüfen, nachdem man F2 auf der Benutzeroberfläche gedrückt hat:
- Command Interface: WICHTIG Diese Einstellung muss auf 'aktiviert' gesetzt werden, damit alles in GeoUTools funktioniert
- UltiDOS: Allow SetDate: Diese Einstellung muss auf 'aktiviert' gesetzt werden, damit GeoUTime die Zeit der Ultimate Real Time Clock ändern kann
- RAM Expansion Unit: Sollte auf 'enabled' gesetzt werden, um das Erkennen, Einbinden und Speichern von GEOS-RAM-Drives zu unterstützen
- REU Size: Stellen Sie hier die gewünschte Größe der REU ein, maximal 16 Megabyte. Abhängig von der GEOS-Konfiguration und der verwendeten GEOS-Distribution können bis zu 4 Megabyte von GEOS selbst genutzt werden, die restlichen 12 Megabyte stehen für RAM Native ram drives zur Verfügung (letztere werden nur in Distributionen wie Wheels oder Megapatch unterstützt)


Einleitung

GeoUTools ist ein kleines Paket von (vorerst) drei Hilfsprogrammen, die die Verwendung der Ultimate II+ Cartridge in GEOS erleichtern.

GeoUTime
Dienstprogramm zum Einstellen der GEOS-Systemzeit (und der UII+ RTC-Uhr) auf die von einem benutzerdefinierten NTP-Server empfangene Zeit.
GeoUTIme ist eine Auto-Exec-Anwendung, die beim Booten automatisch gestartet wird. Aus diesem Grund ist die Verbosity-Einstellung standardmäßig 'Off', so dass nicht bei jedem Start Dialoge angezeigt werden. Sie kann aber zum Debuggen (oder um zu sehen, was passiert) eingeschaltet werden.

GeoUMount
Dieses Dienstprogramm kann zum Swappen und Mounten von virtuellen Festplatten auf der UII+-Cartridge verwendet werden. Dies ist hilfreich, da das UII+-Menü unter GEOS128 im 80-Spalten-Modus nicht verfügbar ist und unter GEOS128 im 40-Spalten-Modus oft abstürzt. Obwohl das Programm hauptsächlich unter GEOS128 nützlich ist, sollte es in allen GEOS-Varianten auf dem C64 und C128 funktionieren, solange der Computer eine UII+ Cartridge hat.
Die Bilder können über einen Dateibrowser ausgewählt werden, der nach den zum Ziellaufwerk passenden Bildtypen filtert.
Bei ausreichender Unterstützung durch eine neue Firmware werden auch GEOS-RAM-Drives, die sich im REU-Speicher befinden, zum Mounten und Speichern von Images unterstützt.
Die Anzahl der Verzeichnisse in diesem Build beläuft sich auf ca. 250 oder so (der Rest des Verzeichnisses wird nicht angezeigt). Das ist, auch in Anbetracht der Filterung auf Verzeichnisse und nur passende Bildtypen, mehr als genug für meine persönlichen Bedürfnisse.

GeoUConfig
Dienstprogramm zum Einstellen der Konfiguration für GeoUTime und GeoUMount.
Für GeoUTime kann die Zeitabfrage eines NTP-Servers aktiviert oder deaktiviert werden, der NTP-Server-Hostname, der Offset von der UTC-Zeit in Sekunden und die Ausführlichkeit ein- oder ausgeschaltet werden.
Für GeoUMount kann die automatische Erkennung gültiger Ziele manuell außer Kraft gesetzt und das Standard-Ziellaufwerk festgelegt werden.

Was befindet sich auf der Diskette

Beim ersten Einhängen des von GitHub heruntergeladenen Disk-Images ist die Disk noch nicht in eine GEOS-Diskette konvertiert, so dass Sie (je nach verwendeter GEOS-Distribution) möglicherweise die Meldung "This is a NON-GEOS disk" erhalten.
Klicken Sie in diesem Dialogfeld auf Ja. Sie sehen dann diese Inhalte auf der Diskette:

- GeoUMount: Werkzeug zur Festplattenmontage
- GeoUTime: NTP Zeitsynchronisationstool
- GeoUConfig: Dienstprogramm zum Einstellen der Konfigurationsoptionen für GeoUTime

Doppelklicken Sie auf das gewünschte Symbol der drei, um das entsprechende Dienstprogramm zu starten.

Beachten Sie, dass sowohl GeoUTime als auch GeoUConfig bei der ersten Verwendung eine Konfigurationsdatei auf der Festplatte anlegen, wenn noch keine vorhanden ist.
Eine neue Datei namens GeoUTimeDat wird hinzugefügt, um Konfigurationsdaten zu speichern.

Es steht Ihnen natürlich frei (und Sie sollten es sogar tun), diese Dateien auf eine andere Festplatte zu kopieren, z. B. auf Ihr Haupt-RAM-Laufwerk.


GeoUConfig Anleitung

Diese Anwendung legt die Konfigurationsoptionen für GeoUTIme fest und speichert sie in der Konfigurationsdatei GeoUTimeDat. Sie erstellt diese Datei beim ersten Start, wenn noch keine Datei vorhanden ist.

Nach dem Start der Anwendung zeigt die Oberfläche die aktuellen Konfigurationseinstellungen an.

Die Optionen können über das Hauptmenü in der linken oberen Ecke erreicht werden. Die Oberfläche wird bei geänderten Einstellungen aktualisiert. Alle Änderungen werden automatisch gespeichert.

Unter "GeoUMount-Konfigurationsdaten" können Sie auch sehen, ob die automatische Erkennung gültiger Laufwerksziele erfolgreich war. Diese Zeile kann die folgenden Optionen anzeigen:
- Autodetection of valid drives succeeded: Die Firmware unterstützt die automatische Erkennung von Laufwerken (Firmware 3.10f und neuer)
- Autodetection might be incorrect:: Die automatische Erkennung wird zwar unterstützt, basiert aber auf der Hardware-ID und nicht auf der Software-ID, so dass sie möglicherweise falsch ist. Überprüfen Sie die erkannten Einstellungen und passen Sie sie bei Bedarf manuell an (Firmware 3.xx bis 3.10e)
- Autodetection not supported in firmware: Die automatische Erkennung wird nicht unterstützt, so dass eine manuelle Auswahl gültiger Ziele erforderlich ist. Die Erkennung wird zwangsweise auf 'Aktiviert' gesetzt (Firmwares vor 3.xx)

Menü GEOS
Optionen in diesem Menü:

Switch 40/80
(Funktioniert nur in GEOS128, wird in GEOS64 ignoriert)
Schaltet zwischen 40- und 80-Spalten-Modus um. Bei der Auswahl wird der andere Modus gewählt und der Bildschirm wird umgeschaltet. Wenn nur ein Monitor verwendet wird, schalten Sie den Monitor auf den entsprechenden anderen Modus um, um ihn zu sehen.

Credits
Wenn Sie diese Option wählen, werden die Credits der Anwendung angezeigt. Drücken Sie OK, um das Dialogfeld für den Abspann zu verlassen.

Exit
Wechselt zum GEOS-Desktop und beendet die Anwendung.

Menü Time
Optionen in diesem Menü:

Enable NTP
Wählen Sie, ob die Zeit mit dem ausgewählten NTP-Server synchronisiert werden soll (Einstellung ist 'Ein'), oder ob nur die GEOS-Systemzeit ohne NTP-Anfrage mit der UII+-Echtzeituhr synchronisiert werden soll.
Wenn Sie diese Option wählen, erscheint ein Dialogfeld. Wählen Sie Ja zum Aktivieren, Nein zum Deaktivieren.

Hostname
Geben Sie den Hostnamen des gewünschten NTP-Servers ein. Der Standardserver ist pool.ntp.org.
Für einen anderen NTP-Server geben Sie einfach dessen Hostnamen (URL oder IP) ein. Die Anwendung prüft nicht, ob die Eingabe gültig ist. Um zu überprüfen, ob der Hostname aufgelöst werden kann und eine erfolgreiche Verbindung zustande kommt, wird der ausführliche Modus empfohlen.
Um den aktuellen Hostnamen beizubehalten, drücken Sie entweder CANCEL oder ENTER.

UTC offset
Bearbeitet den Zeitversatz zur UTC (Universal Standard Time). Der Offset muss in Sekunden angegeben werden. Eine automatische Anpassung an die Sommerzeit (Sommer- und Winterzeit) ist nicht vorgesehen, so dass Sie die Zeitverschiebung bei der Umstellung von Sommer- auf Winterzeit selbst vornehmen müssen.
Der Offset wird in Sekunden angegeben (um den Zeitzonen mit halbstündigem Unterschied Rechnung zu tragen und ggf. eine Feinabstimmung zu ermöglichen), also multiplizieren Sie den UTC-Offset in Stunden mit 3600. Beachten Sie, dass der Offset vorzeichenbehaftet ist, beginnen Sie also mit einem Minus für Offsets negativ zu UTC.
Standardmäßig ist die mitteleuropäische Zeit eingestellt, die einen Offset von 3600 erfordert. Die Einstellung auf mitteleuropäische Sommerzeit würde 7200 erfordern. Ein anderes Beispiel: Die Einstellung auf Eastern Standard Time (EST, Zeitzone für u.a. New York City) wäre UTC -5, also -5*3600 = -18000.
Siehe https://www.timeanddate.com/time/zones/ für alle Zeitverschiebungen in Stunden (multiplizieren Sie mit 3600, um auf Sekunden zu kommen).

Geben Sie den gewünschten Offset ein (achten Sie auf die Vorzeichen, vergessen Sie also nicht das Minuszeichen, wenn die Offsets negativ zu UTC sind) und drücken Sie ENTER. CANCEL, um den aktuellen Wert beizubehalten.

Verbose
Aktivieren oder deaktivieren Sie das Verbosity-Flag. Wenn Sie diese Option aktivieren, gibt GeoUTime eine visuelle Rückmeldung über die ausgeführten Schritte, wenn Sie sie deaktivieren, führt GeoUTime seine Aktionen ohne visuelle Rückmeldung aus.
Die Aktivierung ist nützlich, um Hostnamen und UTC-Offset-Einstellungen zu überprüfen, oder wenn Sie einfach nur neugierig sind. Die Deaktivierung wird für den alltäglichen Gebrauch empfohlen, da die Anwendung beim Booten automatisch ausgeführt wird.
Wählen Sie Ja für den ausführlichen Modus, Nein für den stillen Modus.

Menü Mount
Optionen in diesem Menü:

Enable override
Mit dieser Menüoption können Sie die automatische Erkennung gültiger Ziele für GeoUmount aktivieren oder deaktivieren.
Wählen Sie Ja, um die manuelle Überschreibung zu aktivieren, Nein, um die automatische Zielerkennung zu verwenden.

Set drive
Mit diesem Menüpunkt können Sie über ein Untermenü die Laufwerke A bis D als gültiges Ziel aktivieren oder deaktivieren.
Wählen Sie in diesem Untermenü das gewünschte Laufwerk aus, das Sie ändern möchten.
Wählen Sie Ja, um das Laufwerk als gültiges Ziel einzustellen, Nein, um das Laufwerk nicht als gültiges Ziel zu wählen.

Set target
Mit diesem Menüpunkt können Sie das standardmäßig aktive Ziellaufwerk festlegen. Wenn Sie dies nicht manuell einstellen, wird das Ziellaufwerk als das erste gültige Ziel festgelegt, das Sie finden.
Sie können das Ziellaufwerk über das Untermenü auswählen.
Wenn Sie ein Laufwerk über dieses Untermenü auswählen, wird die Zieleinstellung geändert. Wenn Sie Auto wählen, wird wieder das erste gültige Laufwerk ausgewählt.


GeoUTime Anleitung

Diese Anwendung wird durch die in der Konfigurationsdatei gespeicherten Optionen gesteuert und erfordert keine weitere Benutzerinteraktion.
Da es sich um eine automatische Anwendung handelt, sollte sie automatisch gestartet werden, wenn sie sich auf der Boot-Diskette/dem Boot-Laufwerk befindet. Es wird daher empfohlen, Verbosity nur gelegentlich zu verwenden und es nach der Verwendung auszuschalten. Es ist auch möglich, die Anwendung manuell zu starten.
Wenn Verbosity in den Optionen der Konfigurationsdatei auf "No" gesetzt ist, wird auf dem Bildschirm nichts angezeigt, außer dass der Bildschirm während der Ausführung ausgeblendet und die Systemzeit nach der Rückkehr zum Desktop aktualisiert wird.
Wenn der Ausführlichkeitsmodus auf "Ein" und die NTP-Aktivierung auf "Ein" eingestellt ist, werden Informationen darüber ausgegeben, was die Anwendung gerade tut.

Dieser Modus ist entweder zum Debuggen Ihrer Konfigurationseinstellungen gedacht, oder wenn Sie einfach nur neugierig sind, was passiert:
- Verbindung mit dem ausgewählten Hostnamen des gewünschten NTP-Servers
- Senden der Datenanforderung an diesen Server
- Empfangen der Daten und Interpretieren des Ergebnisses
- Anzeige der empfangenen UNIX-Epoche
- Konvertierung dieser Ausgabe in die Zeit der gewählten Zeitzone unter Verwendung des UTC-Offsets
- Einstellung der UII+-Echtzeituhr auf diese Zeit
- Synchronisierung der GEOS-Systemzeit mit der UII+-Echtzeituhr.
- Bestätigung des Erfolges
Klicken Sie zum Beenden auf das Symbol Beenden.

Beachten Sie, dass bei einem NTP-Verbindungsfehler die GEOS-Uhr mit der unveränderten UII+ RTC-Uhr als Fallback synchronisiert wird.
Wenn die NTP-Aktivierung ausgeschaltet ist, findet nur die Synchronisierung zwischen UII+ RTC und der GEOS-Systemzeit statt, es wird keine NTP-Zeitabfrage durchgeführt.


GeoUMount Anleitung

Diese Anwendung ermöglicht das Einbinden von Disk-Images auf dem UII+-Dateisystem in ein emuliertes UII+-Laufwerk oder ein GEOS-RAM-Laufwerk (falls in der Firmware-Version aktiviert und unterstützt) in GEOS über einen Dateibrowser.

Einschränkungen und Überlegungen zur Anwendung
Die Anwendung stellt sicher, dass nur Disk-Images zum Mounten ausgewählt werden können, die dem Laufwerkstyp des Ziellaufwerks entsprechen, indem die Dateiliste so gefiltert wird, dass nur der entsprechende Imagetyp angezeigt wird.
Dies ist notwendig, da GEOS abstürzt, wenn der Laufwerkstyp eines der aktiven GEOS-Laufwerke spontan geändert wird, z.B. durch das Einbinden eines .D64-Images auf ein emuliertes 1581-Laufwerk. Da GEOS den geänderten Laufwerkstyp nicht kennt, lädt es den falschen Turbo-Load-Code auf das Laufwerk, was GEOS zum Absturz oder Einfrieren bringt. Durch Filterung der Dateiliste sollte dies verhindert werden.

Um die Filterung zu aktivieren, sollten die Dateinamen der Images mit der entsprechenden .dxx-Erweiterung enden:
- .d64 für den Laufwerkstyp 1541 oder 1571
- .g64 für den Laufwerkstyp 1541 oder 1571
- .d71 für den Laufwerkstyp 1571
- .d81 für den Laufwerkstyp 1581
- .dnp für RAM Native Drive Images
Andere Imagetypen werden zur Zeit nicht unterstützt.

Beachten Sie auch, dass Datei- und Verzeichnisnamen nur ausgewählt werden können, wenn sie eine maximale Länge von 20 Zeichen einschließlich Erweiterung haben. Der Grund dafür sind Speicherbeschränkungen: Wenn Sie längere Dateinamen zulassen, können weniger Dateinamen in den Speicher geladen werden. Wenn Sie Verzeichnisse oder Dateien mit längeren Namen auswählen müssen, kürzen Sie diese zunächst auf maximal 20 Zeichen (einschließlich Erweiterung).

Die maximale Anzahl der angezeigten Dateien hängt vom freien Speicherplatz auf dem verwendeten Zielsystem ab, sollte aber normalerweise etwa 250 Dateien betragen. Wenn Sie mehr gültige Bilder oder Unterverzeichnisse in Ihrem aktuellen Verzeichnis haben, wird jeder Eintrag, der über diese Anzahl hinausgeht, nicht angezeigt. Wenn Sie auf diese Verzeichnisse oder Dateien zugreifen müssen, sollten Sie Ihr Verzeichnis umorganisieren und die Dateien in Unterverzeichnissen mit weniger Einträgen unterbringen.

Hauptbenutzeroberfläche
Nach dem Start wird die Hauptbenutzeroberfläche angezeigt.

Hauptmenü
GEOS-Menü
Optionen in diesem Menü:

Switch 40/80
(Funktioniert nur in GEOS128, wird in GEOS64 ignoriert)
Schaltet zwischen 40- und 80-Spalten-Modus um. Bei der Auswahl wird der andere Modus ausgewählt und der Bildschirm wird umgeschaltet. Wenn ein einzelner Monitor verwendet wird, schalten Sie den Monitor auf den entsprechenden anderen Modus um.

Credits
Dieser Menüpunkt zeigt die Credits an.

Exit
Wechselt zum GEOS-Desktop und beendet die Anwendung.

Menü Save REU

Wählen Sie über das Untermenü die zu speichernde Speichergröße aus. Anschließend wird in einem Dialogfeld nach einem Dateinamen für das zu speichernde Bild gefragt. Geben Sie den Dateinamen ein oder drücken Sie Abbrechen, um abzubrechen.
Wenn bereits eine Datei mit diesem Namen im aktiven Verzeichnis vorhanden ist, wird ein neuer Dialog zur Bestätigung oder zum Abbruch aufgefordert.
Je nach ausgewählter Speichergröße kann das Speichern eine Weile dauern. Nach erfolgreichem Abschluss wird eine Meldung wie diese angezeigt.

Auswahl des Ziellaufwerks
Der Auswahlbereich für das Ziellaufwerk befindet sich im oberen rechten Bereich der Hauptbenutzeroberfläche. In diesem Bereich können Sie das Ziellaufwerk auswählen, in das das ausgewählte Disk-Image eingebunden werden soll.
Der Text rechts neben den Symbolen A, B, C und D zeigt an, ob dieser Laufwerksbuchstabe als gültiges Ziel für das Einbinden von Images über die Ultimate II+ Cartridge ausgewählt werden kann. Offensichtlich können nur Laufwerksbuchstaben ausgewählt werden, die aktiven, von der UII+ emulierten Laufwerken entsprechen. Wenn der Laufwerksbuchstabe kein gültiges Ziel ist, wird rechts neben der Schaltfläche der Text "Kein Ziel" angezeigt. Wenn es sich um ein gültiges Ziel handelt, wird der Typ des emulierten Laufwerks angezeigt (derzeit werden die emulierten Laufwerke 1541, 1571 und 1581 unterstützt).

GeoUMount unterstützt derzeit keine Änderung dieser Laufwerkskonfiguration innerhalb der Anwendung. Verwenden Sie die Standard-App Configure, um die Laufwerkskonfiguration zu ändern, falls gewünscht.

Wenn Sie auf das Symbol eines gültigen Laufwerksziels drücken, wird das Ziel auf dieses Laufwerk geändert. Wenn sich der Laufwerkstyp des neuen Ziellaufwerks vom Typ des alten Ziels unterscheidet, wird eine Aktualisierung des aktuell angezeigten Verzeichnisses ausgelöst, um die Filterung für diesen neuen Laufwerkstyp anzuwenden.

Dateibrowser-Navigationssymbole
Dieser Bereich ist der untere rechte Bereich der Hauptbenutzeroberfläche:

Symbol              Funktion
Pfeil nach links    Durch Drücken dieses Symbols wird das angezeigte Verzeichnis in das übergeordnete Verzeichnis des aktuell ausgewählten Verzeichnisses verschoben.
Schrägstrich        Durch Drücken dieses Symbols wird das derzeit angezeigte Verzeichnis in das Stammverzeichnis des Dateisystems verschoben.
Home                Durch Drücken dieses Symbols wird das angezeigte Verzeichnis in das Verzeichnis verschoben, das in den Optionen der UII+-Schnittstelle als Home-Verzeichnis konfiguriert wurde.
Pfeil nach oben     Durch Drücken dieses Symbols gelangen Sie an den Anfang der Dateiliste im aktuell angezeigten Verzeichnis.
Pfeil nach unten    Durch Drücken dieses Symbols gelangen Sie an das Ende der Dateiliste im aktuell angezeigten Verzeichnis.
Speichern Durch     Drücken dieses Symbols wird der Inhalt des aktiven RAM-Laufwerks in einem Abbild gespeichert.

Symbol für Bild speichern
Diese Schaltfläche funktioniert nur, wenn das ausgewählte Ziellaufwerk ein RAM-Laufwerk ist, andernfalls wird ein Klick auf dieses Symbol ignoriert.
Durch Anklicken dieses Symbols kann der Inhalt des Ziel-RAM-Laufwerks in eine Bilddatei im derzeit aktiven Verzeichnis gespeichert werden. Die Dateierweiterung richtet sich nach dem Laufwerkstyp des Ziellaufwerks.
Zuerst wird nach einem Dateinamen gefragt. Geben Sie den Dateinamen ein oder klicken Sie auf Abbrechen, um abzubrechen.
Wenn eine Datei mit diesem Dateinamen bereits existiert, erscheint ein Bestätigungsdialog. Klicken Sie auf Ja, um zu bestätigen und die vorhandene Datei zu überschreiben, auf Nein, um abzubrechen.
Je nach Größe des Bildes kann das Speichern eine Weile dauern. Bei Erfolg wird diese Meldung angezeigt. Klicken Sie auf OK, um fortzufahren.

Dateibrowser: Pfadname
Dieser Abschnitt zeigt den Pfad des aktuell ausgewählten Verzeichnisses neben der Kennung (Ultimate DOS-Version) des UII+-Dateisystems Ultimate Command Interface DOS target.

Dateibrowser: Pfeile zum Blättern in der Zeile
Durch Klicken auf die Pfeilsymbole im Bereich des Dateibrowsers können Sie einen Eintrag nach oben (oberes Pfeilsymbol) oder nach unten (unteres Pfeilsymbol) blättern.

Dateibrowser: Seite nach oben und Seite nach unten
Durch Anklicken des oberen und unteren Scrollbar-Bereichs im Filebrowser-Bereich können Sie eine Seite nach oben (oberer Scrollbar-Bereich) oder nach unten (unterer Scrollbar-Bereich) blättern.

Dateibrowser: Dateiliste
Dieser Bereich zeigt den sichtbaren Teil der Datei- und Verzeichnisliste des aktuell ausgewählten Verzeichnisses. 13 Dateien oder Verzeichnisse können sichtbar angezeigt werden, verwenden Sie die Bildlauf- oder Navigationssymbole/-bereiche, um den sichtbaren Teil zu verschieben.
In der linken Spalte wird der Name der Datei oder des Verzeichnisses angezeigt, in der rechten Spalte der Typ.

Tupfen:

Code    Beschreibung
DIR     Verzeichnis
D64     1541 Bilder mit der Erweiterung .d64
D71     1571 Bilder mit der Endung .d71
D81     1581 Bilder mit der Endung .d81
!TL     Name zu lang, um vollständig angezeigt zu werden. Kann nicht ausgewählt werden, ohne den Namen zu kürzen.
!IS     DNP-Image mit einer Größe, die nicht der Größe des Ziellaufwerks entspricht

Wechseln in ein Unterverzeichnis
Wechseln Sie in ein Unterverzeichnis, indem Sie auf den angezeigten Namen des gewünschten Verzeichnisses klicken.

Abbild zum Einbinden auswählen
Wählen Sie ein zu mountendes Image aus, indem Sie auf einen gültigen Dateinamen klicken (geben Sie also je nach ausgewähltem Ziellaufwerk .d64, .d71 oder .d81 ein).
Bei Erfolg wird ein Dialog angezeigt. Wenn dieser angezeigt wird, ist das ausgewählte Image bereits eingebunden.
Wählen Sie Ja, wenn Sie mit GeoUMount fortfahren wollen (z.B. wenn Sie ein falsches Image ausgewählt haben oder wenn Sie auch Images auf anderen Laufwerken einbinden wollen).
Wählen Sie Nein, um die GeoUMount-Anwendung zu beenden und zum GEOS-Desktop zurückzukehren, wobei das neue Image auf dem ausgewählten Ziellaufwerk eingehängt wird.

Credits

GeoUTools
Werkzeuge für die Verwendung der UII+ Cartridge mit GEOS

Geschrieben im Jahr 2023 von Xander Mol

https://github.com/xahmol/GeoUTools

https://www.idreamtin8bits.com/

Code und Ressourcen von anderen verwendet:

Scott Hutter - Ultimate II+ Bibliothek und GeoUTerm:
https://github.com/xlar54/ultimateii-dos-lib/
(Bibliothek für die UII+ UCI-Funktionen. Und GeoUTerm als GEOS-Beispielanwendung, die diese verwendet)

CC65 Cross-Compiler:
https://cc65.github.io/

Daniel England (Daniel England) - GEOSBuild:
https://github.com/M3wP/GEOSBuild
(Buildtool für GEOS-Disketten)

David Lee - GEOS Image Editor
https://www.facebook.com/groups/704637082964003/permalink/5839146806179646
(Editor für GEOS-Symbole)

Subchrist Software - SpritePad Pro
https://subchristsoftware.itch.io/spritepad-c64-pro
(Editor für die Dateisymbole)

Hitchhikers Guide to GEOS v2022
Berkeley Softworks / Paul B Murdaugh
https://github.com/geos64128/HG2G2022

Beispiel für den Header einer sequentiellen Anwendung von GeoProgrammer
Berkeley Softworks

ntp2ultimate von MaxPlap: Code zum Abrufen der Zeit über NTP
https://github.com/MaxPlap/ntp2ultimate

Heureks: GEOS64 Programmierbeispiele
https://codeberg.org/heurekus/C64-Geos-Programming
(Beispielcode mit C in CC65 zum Schreiben von Anwendungen in GEOS)

Lyonlabs / Cenbe GEOS Ressourcen Seite:
https://www.lyonlabs.org/commodore/onrequest/geos/index.html

Bo Zimmerman GEOS-Ressourcen-Seite:
http://www.zimmers.net/geos/

CBM-Files GEOS-Diskettenabbilder
https://cbmfiles.com/geos/geos-13.php

GEOS - Wheels - GeoWorks - MegaPatch - gateWay - BreadBox Facebook Gruppe
https://www.facebook.com/groups/704637082964003/permalink/5839146806179646

Bart van Leeuwen für das Testen und die Bereitstellung des Device Manager ROM und GEOS RAM Boot

markusC64 (https://github.com/markusC64)
für die Überprüfung des Codes, das Testen und die Anpassung der UII+-Firmware, um die RAM-Disk-Funktionalität zu ermöglichen.

mkslack ( https://github.com/mkslack ) für die Überprüfung und das Testen des Codes

wweicht für die GeoWrite-Version des Handbuchs und das Testen

Forum64.de Forumsthread für Tests und Vorschläge
( https://www.forum64.de/index.php?thread/133681-news-zur-ultimate-1541-ii-und-geos/ )

Gideon Zweijtzer für die Erstellung der Ultimate II+ Cartridge und des Ultimate64, sowie der Ultimate Command Interface, die diese Software ermöglicht.

Getestet mit echter Hardware (C128D, C128DCR, Ultimate 64) mit GEOS128 2.0, GEOS64 2.0 und Megapatch3 128.

izensiert unter der GNU General Public License v3.0

Der Code kann frei verwendet werden, solange Sie
einen Hinweis auf die Originalquelle und den Autor enthält.

DIE PROGRAMME WERDEN IN DER HOFFNUNG VERTEILT, DASS SIE NÜTZLICH SIND,
ABER OHNE JEGLICHE GARANTIE. DIE VERWENDUNG ERFOLGT AUF EIGENE GEFAHR!
