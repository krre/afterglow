use antiq::window::*;

fn main() {
    let mut application_window = ApplicationWindow::new();
    application_window.set_title("Afterglow".to_string());
    println!("{}", application_window.title());
}
