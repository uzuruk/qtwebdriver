// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_TEST_WEBDRIVER_WEBDRIVER_AUTOMATION_H_
#define CHROME_TEST_WEBDRIVER_WEBDRIVER_AUTOMATION_H_

#include <map>
#include <string>
#include <vector>
#include <QtWebKit/QtWebKit>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QObject>
#include <QtCore/QEventLoop>
#include <QtCore/QPointer>
#include <QtCore/QHash>
#include <QtGui/QKeyEvent>
#include <QtCore/QBuffer>
#include <QtXml/QXmlStreamWriter>

#include "base/command_line.h"
#include "base/file_path.h"
// #include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "chrome/common/automation_constants.h"
#include "chrome/test/webdriver/webdriver_logging.h"
#include "ui/base/keycodes/keyboard_codes.h"
#include "chrome/test/automation/automation_json_requests.h"
#include "chrome/test/webdriver/webdriver_basic_types.h"
#include "chrome/test/webdriver/webdriver_element_id.h"
#include "qwebviewext.h"

class AutomationId;
struct MouseEvent;
struct ViewId;
class WebViewLocator;

namespace base {
class DictionaryValue;
class ListValue;
class Value;
}

namespace webdriver {

class Error;
class FramePath;

typedef QHash<QString, QPointer<QWidget> > ElementMap;
typedef QHash<int, QPointer<QWidget> > WindowsMap;

//Notify automation module about end of execution of async script
class JSNotifier : public QObject
{
    Q_OBJECT

public:
    JSNotifier();
    QVariant getResult();
    bool IsCompleted();


public slots:
    void setResult(QVariant result);

signals:
    void completed();

private:
    QVariant res;
    bool isCompleted;
};

// Creates and controls the Chrome instance.
// This class should be created and accessed on a single thread.
// Note: All member functions are void because they are invoked
// by posting a task.
class Automation : public QObject {
    Q_OBJECT
 public:
  struct BrowserOptions {
    BrowserOptions();
    ~BrowserOptions();

    // The command line to use for launching the browser. If no program is
    // specified, the default browser executable will be used.
    CommandLine command;

    // The user data directory to be copied and used. If empty, a temporary
    // directory will be used.
    FilePath user_data_dir;

    // The channel ID of an already running browser to connect to. If empty,
    // the browser will be launched with an anonymous channel.
    std::string channel_id;

    // True if the Chrome process should only be terminated if quit is called.
    // If false, Chrome will also be terminated if this process is killed or
    // shutdown.
    bool detach_process;

    // True if the browser should ignore certificate related errors.
    bool ignore_certificate_errors;

    // The name of window of an already running browser to connect to. If empty,
    // create default window
    std::string browser_start_window;

    // The name of WebView class that will be created on session init. If empty
    // or can't be resolved create default QWebViewExt
    std::string browser_class;
  };

  explicit Automation(const Logger& logger);
  virtual ~Automation();

  // Start the system's default Chrome binary.
  ViewId Init(const BrowserOptions& options, Error **error);

  // Terminates this session and disconnects its automation proxy. After
  // invoking this method, the Automation can safely be deleted.
  void Terminate();

  // Executes the given |script| in the specified frame of the current
  // tab. |result| will be set to the JSON result. Returns true on success.
  void ExecuteScript(const ViewId& view_id,
                     const FramePath& frame_path,
                     const std::string& script,
                     std::string* result,
                     bool isAsync, Error **error);

  // Sends a webkit key event to the current browser. Waits until the key has
  // been processed by the web page.
  void SendWebKeyEvent(const ViewId& view_id,
                       const KeyEvent& key_event,
                       Error** error);

  void SendNativeElementWebKeyEvent(const ViewId &view_id,
                                    const ElementId &element,
                                    const KeyEvent &key_event,
                                    Error **error);

  // Sends an OS level key event to the current browser. Waits until the key
  // has been processed by the browser.
  void SendNativeKeyEvent(const ViewId& view_id,
                          ui::KeyboardCode key_code,
                          int modifiers,
                          Error** error);

  // Sends a web mouse event to the given view. Waits until the event has
  // been processed by the view.
  void SendWebMouseEvent(const ViewId& view_id,
                         const MouseEvent& event,
                         Error** error);

  // Drag and drop the file paths to the given location.
  void DragAndDropFilePaths(const ViewId& view_id,
                            const Point& location,
                            const std::vector<FilePath::StringType>& paths,
                            Error** error);

  // Captures a snapshot of the tab to the specified path.  The  PNG will
  // contain the entire page, including what is not in the current view
  // on the  screen.
  void CaptureEntirePageAsPNG(
      const ViewId& view_id, const FilePath& path, Error** error);

  void NavigateToURL(
      const ViewId& view_id, const std::string& url, Error** error);
  void NavigateToURLAsync(
      const ViewId& view_id, const std::string& url, Error** error);
  void GoForward(const ViewId& view_id, Error** error);
  void GoBack(const ViewId& view_id, Error** error);
  void Reload(const ViewId& view_id, Error** error);

  void GetCookies(const ViewId& view_id,
                  const std::string& url,
                  base::ListValue** cookies,
                  Error** error);
  void DeleteCookie(const ViewId& view_id,
                    const std::string& url,
                    const std::string& cookie_name,
                    Error** error);
  void SetCookie(const ViewId& view_id,
                 const std::string& url,
                 base::DictionaryValue* cookie_dict,
                 Error** error);

  // TODO(kkania): All of these mouse commands are deprecated and should be
  // removed when chrome build 1002 is no longer supported.
  // Use SendWebMouseEvent instead.
  void MouseMoveDeprecated(const ViewId& view_id,
                           const Point& p,
                           Error** error);
  void MouseClickDeprecated(const ViewId& view_id,
                            const Point& p,
                            automation::MouseButton button,
                            Error** error);
  void MouseDragDeprecated(const ViewId& view_id,
                           const Point& start,
                           const Point& end,
                           Error** error);
  void MouseButtonDownDeprecated(const ViewId& view_id,
                                 const Point& p,
                                 Error** error);
  void MouseButtonUpDeprecated(const ViewId& view_id,
                               const Point& p,
                               Error** error);
  void MouseDoubleClickDeprecated(const ViewId& view_id,
                                  const Point& p,
                                  Error** error);

  // Get info for all views currently open.
  void GetViews(std::vector<ViewId>* views, Error** error);

  // Check if the given view exists currently.
  void DoesViewExist(ViewId *view_id, bool* does_exist, Error** error);

  // Closes the given view.
  void CloseView(const ViewId& view_id, Error** error);

  // Gets the bounds for the given view.
  void GetViewBounds(const ViewId &view_id, Rect *bounds, Error **error);

  // Gets view title
  void GetViewTitle(const ViewId &view_id, std::string* title, Error **error);

  // Sets the bounds for the given view. The position should be in screen
  // coordinates, while the size should be the desired size of the view.
  void SetViewBounds(const ViewId& view_id,
                     const Rect& bounds,
                     Error** error);

  // Maximizes the given view.
  void MaximizeView(const ViewId& view_id, Error** error);

  // Gets the active JavaScript modal dialog's message.
  void GetAppModalDialogMessage(const ViewId& view_id, std::string* message, Error** error);

  // Accepts or dismisses the active JavaScript modal dialog.
  void AcceptOrDismissAppModalDialog(const ViewId& view_id, bool accept, Error** error);

  // Accepts an active prompt JavaScript modal dialog, using the given
  // prompt text as the result of the prompt.
  void AcceptPromptAppModalDialog(const ViewId& view_id,
                                  const std::string& prompt_text,
                                  Error** error);

  // Gets the version of the runing browser.
  void GetBrowserVersion(std::string* version);

  // Waits for all views to stop loading.
  void WaitForAllViewsToStopLoading(Error** error);

  // Gets the current geolocation.
  void GetGeolocation(scoped_ptr<base::DictionaryValue>* geolocation,
                      Error** error);

  // Overrides the current geolocation.
  void OverrideGeolocation(const base::DictionaryValue* geolocation,
                           Error** error);

  // set unique id to the frame
  void AddIdToCurrentFrame(const ViewId &view_id, const FramePath &frame_path, Error **error);

  // set text into Prompt text field
  void SetAlertPromptText(const ViewId& view_id, const std::string& text, Error **error);


  // get native element size
  void GetNativeElementSize(const ViewId& view_id,
                         const ElementId& element,
                         Size* size,
                         Error** error);

  // find native element
  void FindNativeElement(const ViewId& view_id,
                         const ElementId& root_element,
                         const std::string& locator,
                         const std::string& query,
                         ElementId* element,
                         Error** error);

  void FindNativeElements(const ViewId& view_id,
                         const ElementId& root_element,
                         const std::string& locator,
                         const std::string& query,
                         std::vector<ElementId>* elements,
                         Error** error);

  void GetNativeElementWithFocus(const ViewId& view_id,
                         ElementId* element,
                         Error** error);

  void GetNativeElementLocation(const ViewId& view_id,
                         const ElementId& element,
                         Point* location,
                         Error** error);

  void GetNativeElementProperty(const ViewId& view_id,
                         const ElementId& element,
                         const std::string& name,
                         base::Value** value,
                         Error** error);

  void NativeElementEquals(const ViewId& view_id,
                         const ElementId& element1,
                         const ElementId& element2,
                         bool* is_equals,
                         Error** error);

  void GetNativeElementClickableLocation(const ViewId& view_id,
                         const ElementId& element,
                         Point* location,
                         Error** error);

  void GetNativeElementLocationInView(const ViewId& view_id,
                         const ElementId& element,
                         Point* location,
                         Error** error);

  void ClearNativeElement(const ViewId& view_id,
                         const ElementId& element,
                         Error** error);

  void IsNativeElementDisplayed(const ViewId& view_id,
                         const ElementId& element,
                         bool ignore_opacity,
                         bool* is_displayed,
                         Error** error);

  void IsNativeElementEnabled(const ViewId& view_id,
                         const ElementId& element,
                         bool* is_enabled,
                         Error** error);

  void IsNativeElementSelected(const ViewId& view_id,
                         const ElementId& element,
                         bool* is_selected,
                         Error** error);

  void GetNativeElementText(const ViewId &view_id,
                         const ElementId& element,
                         std::string* element_text,
                         Error **error);
						 
  void GetNativeSource(const ViewId& view_id,
                         base::Value** result,
                         Error** error);


 private:
  Error* DetermineBuildNumber();
  Error* CheckVersion(int min_required_build_no,
                      const std::string& error_msg);
  Error* CheckAlertsSupported();
  Error* CheckAdvancedInteractionsSupported();
  Error* CheckNewExtensionInterfaceSupported();
  Error* CheckGeolocationSupported();
  Error* CheckMaximizeSupported();
  Error* IsNewMouseApiSupported(bool* supports_new_api);

  QPoint ConvertPointToQPoint(const Point& p);
  Rect ConvertQRectToRect(const QRect &rect);
  QRect ConvertRectToQRect(const Rect& rect);
  Qt::MouseButton ConvertMouseButtonToQtMouseButton(automation::MouseButton button);
  QWebFrame* FindFrameByPath(QWebFrame* parent, const FramePath &frame_path);
  QWebFrame* FindFrameByMeta(QWebFrame* parent, const FramePath &frame_path);
  QWidget* GetNativeElement(const ViewId &view_id, const ElementId &element);
  bool FilterNativeWidget(const QWidget* widget, const std::string& locator, const std::string& query);
  QString GenerateElementKey(const QWidget* widget);
  QKeyEvent ConvertToQtKeyEvent(const KeyEvent &key_event);
  void BuildKeyMap();
  QWidget *checkView(const ViewId &view_id);
  void createUIXML(QWidget *parent, QIODevice *buff, ElementMap* elementsMap, Error **error, bool needAddWebSource = false);
  void FindNativeElementByXpath(QWidget *parent, ElementMap* elementsMap, const std::string &query, std::vector<ElementId>* elements, Error **error);
  void addWidgetToXML(QWidget* parent, ElementMap *elementsMap, QXmlStreamWriter *writer, bool needAddWebSource = false);
  int checkViewInMap(QWidget* view);

  const Logger& logger_;
  int build_no_;
  scoped_ptr<base::DictionaryValue> geolocation_;
  int sessionId;
  QEventLoop loop;
  bool isLoading;
  QMap<int, int> keyMap;
  QHash<QString, ElementMap* > windowsElementMap;
  WindowsMap windowsMap;

  DISALLOW_COPY_AND_ASSIGN(Automation);

private slots:
  void pageLoadStarted();
  void pageLoadFinished();

};

}  // namespace webdriver

#endif  // CHROME_TEST_WEBDRIVER_WEBDRIVER_AUTOMATION_H_
