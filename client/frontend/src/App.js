import React, { useState, useEffect } from "react";
import axios from "axios";
import "./App.css";
import Typewriter from "./components/Typewriter";

function App() {
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [confirmPassword, setConfirmPassword] = useState("");
  const [isSignUp, setIsSignUp] = useState(false);
  const appTitle = "Venora";

  const handleSubmit = async (e) => {
    e.preventDefault();
    // fetch("http://127.0.0.1:5000/register", {
    //   method: "POST",
    //   headers: {
    //     "Content-Type": "application/json",
    //   },
    //   body: JSON.stringify({ username, password }),
    // })
    //   .then((response) => response.json())
    //   .then((data) => console.log(data))
    //   .catch((error) => console.error("Error:", error));
    try {
      const response = await axios.post("/register", { username, password });
      console.log("Registration success:", response.data);
    } catch (error) {
      console.error("Error during registration:", error);
    }
  };

  return (
    <div className="App">
      <header className="App-header">
        <div className="title-container">
          <h1 className="app-title">
            <Typewriter text={appTitle} delay={500} />
          </h1>
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
