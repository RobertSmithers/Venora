import React, { useState, useEffect } from "react";
import "./App.css";

function App() {
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [confirmPassword, setConfirmPassword] = useState("");
  const [isSignUp, setIsSignUp] = useState(false);
  const [title, setTitle] = useState("");
  const appTitle = "Venora";
  const appSubtitle = "Dotcom Offensive Security Suite";

  useEffect(() => {
    let i = 0;
    const typingInterval = setInterval(() => {
      if (i < appTitle.length) {
        setTitle((prev) => prev + appTitle[i]);
        i++;
      } else {
        clearInterval(typingInterval);
      }
    }, 150);
    return () => clearInterval(typingInterval);
  }, []);

  const handleSubmit = (e) => {
    e.preventDefault();
    console.log("Username:", username);
    console.log("Password:", password);
    if (isSignUp) {
      console.log("Confirm Password:", confirmPassword);
    }
  };

  return (
    <div className="App">
      <header className="App-header">
        <div className="title-container">
          <h1 className="app-title">{title}</h1>
        </div>
        <div className={`form-container ${isSignUp ? "signup-mode" : ""}`}>
          <form className="auth-form" onSubmit={handleSubmit}>
            <h2>{isSignUp ? "Sign Up" : "Login"}</h2>
            <div className="input-container">
              <input
                type="text"
                placeholder="Username"
                value={username}
                onChange={(e) => setUsername(e.target.value)}
                required
              />
            </div>
            <div className="input-container">
              <input
                type="password"
                placeholder="Password"
                value={password}
                onChange={(e) => setPassword(e.target.value)}
                required
              />
            </div>
            {isSignUp && (
              <div className="input-container">
                <input
                  type="password"
                  placeholder="Confirm Password"
                  value={confirmPassword}
                  onChange={(e) => setConfirmPassword(e.target.value)}
                  required
                />
              </div>
            )}
            <button type="submit" className="btn primary-btn">
              {isSignUp ? "Register" : "Login"}
            </button>
            <button
              type="button"
              className="btn toggle-btn"
              onClick={() => setIsSignUp(!isSignUp)}
            >
              {isSignUp
                ? "Already have an account? Login"
                : "Don't have an account? Sign Up"}
            </button>
          </form>
        </div>
      </header>
    </div>
  );
}

export default App;
