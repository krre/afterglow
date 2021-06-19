use antiq::entity::Application;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let app = Application::new();
    app.run()
}
