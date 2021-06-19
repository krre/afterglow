use antiq::entity::Application;
use antiq::widget::ApplicationWindow;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let mut app_window = ApplicationWindow::new();
    app_window.set_title("Afterglow");

    let app = Application::new();
    app.run()
}
