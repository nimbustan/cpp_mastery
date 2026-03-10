/**
 * @file module_02_oop/01_classes_objects.cpp
 * @brief OOP Part 1: Architecture of Classes, Objects, and Access Specifiers / OOP Kısım 1:
 * Sınıfların, Nesnelerin ve Erişim Belirleyicilerin Mimarisi
 *
 * @details
 * =============================================================================
 * [THEORY: What are Classes and Objects? Why do we need them? / TEORİ: Sınıf ve Nesne Nedir?]
 * =============================================================================
 * EN: In C++, primitive types (int, float, char) are often not enough to represent complex,
 * real-world entities. For example, a 'TemperatureSensor' in an automotive ECU isn't just an
 * integer. It has a 'sensorName' (string), 'currentTempC' (int), and behaviors like
 * 'reportOverheat()' (function).
 *
 * A 'Class' is a blueprint or a template. It takes up absolutely NO memory by itself. It just
 * tells the compiler: "If someone creates a TemperatureSensor, it will look like this."
 *
 * An 'Object' (or Instance) is the actual creation of that blueprint in the RAM. When we say
 * `TemperatureSensor s1;`, memory is allocated, and the Object comes to life.
 *
 * TR: C++'da ilkel tipler (int, float, char) çoğu zaman karmaşık, gerçek dünya varlıklarını
 * temsil etmek için yeterli değildir. Örneğin, bir otomotiv ECU'daki 'SıcaklıkSensörü' sadece
 * bir tam sayı değildir. Sensörün bir 'Adı' (string), 'MevcutSıcaklık' (int) ve
 * 'asırıIsınmaRaporla()' gibi davranışları (fonksiyon) vardır.
 *
 * 'Sınıf' (Class) bir donanım mimari çizimi, bir şablondur. Kendi başına RAM'de HİÇBİR yer
 * kaplamaz. Sadece derleyiciye şunu söyler: "Eğer biri bir SıcaklıkSensörü oluşturursa, yapısı
 * böyle olacak."
 *
 * 'Nesne' (Object veya Instance) ise o şablonun RAM'deki canlı kanlı üretimidir.
 * `TemperatureSensor s1;` dediğimiz anda bellekte yer ayrılır ve Nesne hayata gözlerini açar.
 *
 * =============================================================================
 * [THEORY: Why use Access Specifiers? / TEORİ: Erişim Belirleyiciler Neden Var?]
 * =============================================================================
 * EN: Why do we make data 'private' in a class? Why can't anyone just say `p1.health = -500;`?
 * Because we want to protect the system's safety and integrity. Forcing the outside world to
 * communicate with the variables ONLY through our permitted 'public' functions is called
 * Encapsulation.
 *
 * TR: Bir sınıftaki verileri neden 'private' (özel) yaparız? Neden herkes doğrudan `p1.health =
 * -500;` diyemesin? Çünkü sistemin güvenliğini ve bütünlüğünü korumak isteriz. Dış dünyanın
 * sadece bizim izin verdiğimiz 'public' (genel) fonksiyonlar üzerinden içerikteki değişkenlerle
 * iletişim kurmasına Encapsulation (Kapsülleme) denir.
 *
 * =============================================================================
 * [CPPREF DEPTH: Memory Layout & POD (Plain Old Data) Types / CPPREF DERİNLİK: Bellek Düzeni ve
 * POD (Düz Eski Veri) Tipleri]
 * =============================================================================
 * EN: According to cppreference, as soon as you add `private` access specifiers or `virtual`
 * functions, the Class is NO LONGER a "Standard Layout" (POD) type. This means you cannot safely
 * use `memcpy()` or pass it to a C-language API.
 *
 * TR: cppreference der ki: Bir sınıfa `private` değişkenler veya `virtual` fonksiyonlar
 * eklerseniz, o sınıf artık C-uyumlu Standart Bellek Düzenine (POD) sahip değildir! Yani bu
 * nesneyi `memcpy()` ile kopyalamak veya ağ üzerinden byte dizisi olarak ham haliyle göndermek
 * Tanımsız Davranış (UB) yaratır!
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>

/**
 * @class TemperatureSensor
 * @brief Represents an automotive temperature sensor (e.g. EGT, coolant). / Bir otomotiv
 * sıcaklık sensörünü temsil eder (örn. EGT, soğutma suyu).
 *
 * @note
 * EN: Design Philosophy: Data is kept private. It cannot be directly modified from the outside.
 * Anyone who wants to change it must use our 'public' methods.
 * TR: Tasarım Felsefesi: Veriler private (saklı) tutulmuştur. Dışarıdan doğrudan
 * değiştirilemezler. Değişiklik yapmak isteyen, 'public' metodlarımızı kullanmak zorundadır.
 */
class TemperatureSensor {
  // EN: 'private' section is INACCESSIBLE from outside the class.
  // TR: 'private' bölümüne sınıfın DIŞINDAN erişilemez. Sadece sınıfın içindeki fonksiyonlar
  // görebilir.
private:
  std::string sensorName; /**< Sensor ID. */
  int currentTempC; /**< Current temperature in °C. */

  // EN: 'public' section is accessible from anywhere. It's the interface to the outside world.
  // TR: 'public' bölümüne her yerden erişilebilir. Sınıfın dış dünyayla olan arayüzüdür.
public:
  /**
   * @brief Constructor
   * TR: Yapıcı Fonksiyon
   *
   * @details
   * EN: [THEORY: What is a Constructor?] It's the first function that runs automatically the
   * moment an Object is created in RAM. It has the EXACT same name as the Class and NO return
   * type. Its purpose is to put the object in a "ready and valid" state. Member Initialization
   * List (":"): Used for performance. Instead of assigning inside the body "{}", we write
   * directly to memory upon creation.
   *
   * TR: [TEORİ: Constructor Nedir?] Nesne RAM'de "yaratıldığı anda" ilk çalışacak olan
   * fonksiyondur. Sınıf ile tamamen AYNI isme sahiptir ve herhangi bir dönüş tipi yoktur. Amacı
   * nesneyi kullanıma "hazır ve geçerli" (valid) bir hale getirmektir. Üye Başlatma Listesi
   * (Member Initialization List - ":"): Performans için önemlidir. Süslü parantez içinde atamak
   * yerine doğrudan belleğe yazmayı sağlar.
   *
   * @param pName Sensor identifier / Sensörün başlangıç adı
   * @param pTemp Initial temperature °C / Başlangıç sıcaklığı (°C)
   */
  TemperatureSensor(std::string pName, int pTemp)
      : sensorName(pName), currentTempC(pTemp) {
    std::cout << "[SYSTEM] Constructor Called: " << sensorName
              << " initialized at " << currentTempC << " °C." << std::endl;
  }

  /**
   * @brief Reports a temperature change safely.
   * TR: Sıcaklık değişimini güvenli bir şekilde raporlar.
   *
   * @details
   * EN: The power of Encapsulation! By preventing direct access to 'currentTempC', we can
   * enforce an overheat threshold (120 °C) right here.
   * TR: Kapsülleme (Encapsulation) gücü! Sıcaklığı direkt değiştirmek yerine bu fonksiyonu
   * kullandığımız için, aşırı ısınma eşiğini (120 °C) burada zorla uygulayabiliriz.
   *
   * @param delta Temperature change in °C / Sıcaklık değişimi (°C)
   */
  void reportTemperatureChange(int delta) {
    std::cout << "[SENSOR] " << sensorName << " temp changes by +"
              << delta << " °C." << std::endl;
    currentTempC += delta;

    // EN: Our safety net — overheat threshold!
    // TR: Güvenlik eşiği!
    if (currentTempC > 120) {
      std::cout << "[ALARM] " << sensorName
                << " OVERHEAT! Temp: " << currentTempC
                << " °C (Limit: 120 °C)" << std::endl;
    } else {
      std::cout << "[STATUS] " << sensorName
                << " current temp: " << currentTempC << " °C" << std::endl;
    }
  }
};

/**
 * @brief Main function demonstrating Class instantiation.
 * TR: Sınıf nesnesi yaratılmasını gösteren ana fonksiyon.
 */
int main() {
  std::cout << "=== OOP Part 1: Classes & Objects ===" << std::endl;

  // EN: 1. Object Instantiation
  // TR: 1. Nesne Yaratma
  // EN: Creating a sensor object from the blueprint. Constructor runs here!
  // TR: Şablondan bir sensör nesnesi yaratıyoruz. Yapıcı burada çalışır!
  TemperatureSensor egtSensor("EGT_Cyl1", 85);

  // EN: 2. Encapsulation
  // TR: 2. Kapsülleme ve Sınırlar
  // EN: ERROR! 'currentTempC' is private.
  // TR: HATA! 'currentTempC' private (ozel) veridir. egtSensor.currentTempC = 9999;

  // EN: Using the safe Public Method
  // TR: Güvenli Public Metodu kullanıyoruz.
  egtSensor.reportTemperatureChange(15);
  // Trying to push temp over the limit / Sıcaklığı limit üstüne çıkarmayı deniyoruz.
  egtSensor.reportTemperatureChange(25);

  return 0;
}
