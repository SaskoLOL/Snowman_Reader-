#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

#define BUTTON_NEXT 13    
#define BUTTON_PREV 12  

const int textSize = 2;
const int SCREEN_WIDTH = 240;
const int SCREEN_HEIGHT = 320;
const int lineHeight = 16 * textSize;
const int wordsPerPage = 50;

const char* fullText =
  "This is a demonstration of displaying text on a 2.8 inch TFT screen using the Seeed XIAO RP2040. "
  "We are dividing the content into pages, where each page shows up to fifty words. "
  "This makes reading easier, and it's especially helpful for instructions, e-books, or long messages. "
  "Use the two buttons to go forward or backward. "
  "This setup does not require an SD card and is ideal for simple embedded text displays. "
  "Enjoy scrolling through the text on your device. "
  "This example uses the TFT_eSPI library which is fast and compatible with many SPI displays. "
  "You can modify the code to load more or less words per page, or change how the buttons behave. "
  "The Seeed XIAO RP2040 is a great little board for compact projects like this. "
  "Thank you for using this demo, and happy building!";
  
String pages[20];
int totalPages = 0;
int currentPage = 0;

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_PREV, INPUT_PULLUP);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(textSize);
  tft.setTextColor(TFT_WHITE);

  paginateText();
  displayPage(currentPage);
}

void loop() {
  if (digitalRead(BUTTON_NEXT) == LOW) {
    delay(200);
    if (currentPage < totalPages - 1) {
      currentPage++;
      displayPage(currentPage);
    }
  }

  if (digitalRead(BUTTON_PREV) == LOW) {
    delay(200);
    if (currentPage > 0) {
      currentPage--;
      displayPage(currentPage);
    }
  }
}

void paginateText() {
  String text = String(fullText);
  text.trim();

  int wordCount = 0;
  String currentWord = "";
  String currentPageText = "";

  for (unsigned int i = 0; i < text.length(); i++) {
    char c = text[i];
    if (c == ' ' || c == '\n') {
      if (currentWord.length() > 0) {
        currentPageText += currentWord + " ";
        wordCount++;
        currentWord = "";
        if (wordCount == wordsPerPage) {
          pages[totalPages++] = currentPageText;
          currentPageText = "";
          wordCount = 0;
        }
      }
    } else {
      currentWord += c;
    }
  }

  if (currentWord.length() > 0) {
    currentPageText += currentWord + " ";
    wordCount++;
  }

  if (currentPageText.length() > 0) {
    pages[totalPages++] = currentPageText;
  }
}

// --- Display a page of text on TFT ---
void displayPage(int pageNum) {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(textSize);
  tft.setTextColor(TFT_WHITE);

  String pageText = pages[pageNum];
  int y = 0;
  int lineStart = 0;

  while (lineStart < pageText.length()) {
    int lineEnd = lineStart + 30;
    if (lineEnd > pageText.length()) lineEnd = pageText.length();

    int splitAt = pageText.lastIndexOf(' ', lineEnd);
    if (splitAt <= lineStart) splitAt = lineEnd;

    String line = pageText.substring(lineStart, splitAt);
    tft.setCursor(0, y);
    tft.println(line);
    y += lineHeight;

    lineStart = splitAt + 1;
  }

  tft.setTextSize(1);
  String pageStr = "Page " + String(pageNum + 1) + "/" + String(totalPages);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(pageStr, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor(SCREEN_WIDTH - w - 5, SCREEN_HEIGHT - h - 5);
  tft.print(pageStr);
}
