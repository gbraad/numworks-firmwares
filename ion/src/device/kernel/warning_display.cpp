#include "warning_display.h"
#include <drivers/keyboard.h>
#include <drivers/display.h>
#include <ion/display.h>
#include <kandinsky/font.h>
#include <kandinsky/rect.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace WarningDisplay {

void drawString(const char * text, KDCoordinate * yOffset, const KDFont * font) {
  KDSize glyphSize = font->glyphSize();
  KDFont::RenderPalette palette = font->renderPalette(KDColorBlack, KDColorWhite);
  KDFont::GlyphBuffer glyphBuffer;
  size_t len = strlen(text);
  KDPoint position((Ion::Display::Width - len*glyphSize.width())/2, *yOffset);

  while (*text) {
    font->setGlyphGrayscalesForCharacter(*text++, &glyphBuffer);
    font->colorizeGlyphBuffer(&palette, &glyphBuffer);
    // Push the character on the screen
    Ion::Device::Display::pushRect(KDRect(position, glyphSize), glyphBuffer.colorBuffer());
    position = position.translatedBy(KDPoint(glyphSize.width(), 0));
  }
  *yOffset += glyphSize.height();
}

void showMessage(const char * const * messages, int numberOfMessages) {
  KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
  Ion::Device::Display::pushRectUniform(screen, KDColorWhite);
  const char * title = messages[0];
  KDCoordinate currentHeight = 60;
  drawString(title, &currentHeight, KDFont::LargeFont);
  currentHeight += KDFont::LargeFont->glyphSize().height();
  for (int j = 1; j < numberOfMessages; j++) {
    const char * message = messages[j];
    // Move draw string here are you're the only on to use it!
    drawString(message, &currentHeight, KDFont::SmallFont);
  }
}

void waitUntilKeyPress() {
  while (1) {
    Ion::Keyboard::State s = Keyboard::scan();
    if (s != 0) {
      break;
    }
  }
}

constexpr static int sUnauthenticatedUserlandNumberOfMessages = 6;
constexpr static const char * sUnauthenticatedUserlandMessages[sUnauthenticatedUserlandNumberOfMessages] = {
  "UNOFFICIAL SOFTWARE",
  "Caution: You are using an unofficial",
  "software. This software does not match",
  "the requirements of some exams.",
  "NumWorks is not responsible for problems",
  "that arise from the use of this software."
};

void unauthenticatedUserland() {
  showMessage(sUnauthenticatedUserlandMessages, sUnauthenticatedUserlandNumberOfMessages);
  waitUntilKeyPress();
}

constexpr static int sKernelUpgradeRequiredNumberOfMessages = 8;
constexpr static const char * sKernelUpgradeRequiredMessages[sKernelUpgradeRequiredNumberOfMessages] = {
  "OFFICIAL UPGRADE REQUIRED",
  "The software you were about",
  "to execute requires a more",
  "recent authentified kernel",
  "to be executed. Please upgrade",
  "your calculator with the last",
  "official software before trying",
  "again."
};

void upgradeRequired() {
  // TODO image instead of words
  showMessage(sKernelUpgradeRequiredMessages, sKernelUpgradeRequiredNumberOfMessages);
  waitUntilKeyPress();
}

constexpr static int sExternalAppsAvailableNumberOfMessages = 7;
constexpr static const char * sExternalAppsAvailableMessages[sExternalAppsAvailableNumberOfMessages] = {
  "EXTERNAL APPS",
  "You installed external applications",
  "to the official sofware.",
  "Some features may be therefore",
  "unavailable.",
  "Every permission will be restored",
  "when resetting the calculator.",
};

void externalAppsAvailable() {
  showMessage(sExternalAppsAvailableMessages, sExternalAppsAvailableNumberOfMessages);
  waitUntilKeyPress();
}

}
}
}