use antiq::core::application;
use antiq::window::*;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let mut application_window = ApplicationWindow::new();
    application_window.set_title("Afterglow");
    application::run()
}
