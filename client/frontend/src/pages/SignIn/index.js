import React from "react";
import PropTypes from "prop-types";

import SignInForm from "./components/SignInForm";
import ConnectionError from "./components/ConnectionError";

function SignInSection({ isConnected, connectFunction }) {
  // const appTitle = "Venora";

  // useEffect(() => {
  //   let i = 0;
  //   const typingInterval = setInterval(() => {
  //     setTitle((prev) => {
  //       if (i < appTitle.length) {
  //         const newTitle = prev + appTitle[i];
  //         i++;
  //         return newTitle;
  //       } else {
  //         clearInterval(typingInterval);
  //         return prev;
  //       }
  //     });
  //   }, 150);
  //   return () => clearInterval(typingInterval);
  // }, []);

  return (
    <div className="App">
      <header className="App-header">
        <div className="title-container">
          <h1 className="app-title">{/* <Typewriter text={appTitle} delay={500} /> */}</h1>
        </div>
        {isConnected ? <SignInForm /> : <ConnectionError onRetry={connectFunction} />}
      </header>
    </div>
  );
}

SignInSection.propTypes = {
  isConnected: PropTypes.bool.isRequired,
  connectFunction: PropTypes.func.isRequired,
};

export default SignInSection;
