import type { Metadata } from 'next';
import './globals.css';
export const metadata: Metadata = {title:'ITO · Thermal Observatory',description:'Local commissioning dashboard for the revision-L ITO controller'};
export default function Layout({children}:Readonly<{children:React.ReactNode}>) {return <html lang="en"><body>{children}</body></html>;}
