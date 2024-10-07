import { useEffect, useState } from "react";

import { Routes, Route, Navigate, useLocation } from "react-router-dom";
import CssBaseline from "@mui/material/CssBaseline";

import axios from "axios";
import Cookies from "js-cookie";
import { v4 as uuidv4 } from "uuid";

import SignInPage from "layouts/pages/authentication/sign-in";

export default function App() {
  const [isConnected, setIsConnected] = useState(true); // Track connection status to the server
  const { pathname } = useLocation();

  const checkConnection = async () => {
    try {
      let userId = Cookies.get("userId");
      if (!userId) {
        userId = uuidv4();
        Cookies.set("userId", userId);
      }

      console.log("User ID is:", userId);

      await axios.post(
        "/connect",
        {},
        {
          headers: {
            "X-User-Id": userId,
          },
        }
      ); // Connect to the server

      setIsConnected(true);
      return true;
    } catch (error) {
      console.error("Connection failed:", error);
      setIsConnected(false);
      return false;
    }
  };

  // Setting page scroll to 0 when changing the route
  useEffect(() => {
    document.documentElement.scrollTop = 0;
    document.scrollingElement.scrollTop = 0;
    checkConnection();
  }, [pathname]);

  return (
    <>
      <CssBaseline />
      <Routes>
        {/* {getRoutes(routes)} */}
        <Route
          path="/SignIn"
          element={<SignInPage isConnected={isConnected} connectFunction={checkConnection} />}
        />
        <Route path="*" element={<Navigate to="/SignIn" />} />
      </Routes>
    </>
  );
}
