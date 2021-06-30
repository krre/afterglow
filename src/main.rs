use antiq::entity::Application;
use antiq::widget::ApplicationWindow;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let mut app_window = Box::new(ApplicationWindow::new());
    app_window.set_title("Afterglow");

    let mut app = Application::new();
    app.add_window(app_window);

    app.run()
}
